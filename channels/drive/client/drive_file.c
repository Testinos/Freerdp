/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * File System Virtual Channel
 *
 * Copyright 2010-2012 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 * Copyright 2010-2011 Vic Lee
 * Copyright 2012 Gerald Richter
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef _WIN32
#define __USE_LARGEFILE64
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE

#include <sys/time.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include <winpr/crt.h>
#include <winpr/file.h>

#include <winpr/stream.h>
#include <freerdp/channels/rdpdr.h>
#include <freerdp/utils/svc_plugin.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include "drive_file.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4244)
#endif

static void drive_file_fix_path(char* path)
{
	int i;
	int length;

	length = strlen(path);

	for (i = 0; i < length; i++)
	{
		if (path[i] == '\\')
			path[i] = '/';
	}

#ifdef WIN32
	if ((length == 3) && (path[1] == ':') && (path[2] == '/'))
		return;
#else
	if ((length == 1) && (path[0] == '/'))
		return;
#endif

	if ((length > 0) && (path[length - 1] == '/'))
		path[length - 1] = '\0';
}

static char* drive_file_combine_fullpath(const char* base_path, const char* path)
{
	char* fullpath;

	fullpath = (char*) malloc(strlen(base_path) + strlen(path) + 1);
	strcpy(fullpath, base_path);
	strcat(fullpath, path);
	drive_file_fix_path(fullpath);

	return fullpath;
}

static BOOL drive_file_remove_dir(const char* path)
{
	DIR* dir;
	char* p;
	struct STAT st;
	struct dirent* pdirent;
	BOOL ret = TRUE;

	dir = opendir(path);

	if (dir == NULL)
		return FALSE;

	pdirent = readdir(dir);

	while (pdirent)
	{
		if (strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0)
		{
			pdirent = readdir(dir);
			continue;
		}

		p = (char*) malloc(strlen(path) + strlen(pdirent->d_name) + 2);
		sprintf(p, "%s/%s", path, pdirent->d_name);

		if (STAT(p, &st) != 0)
		{
			DEBUG_WARN("stat %s failed.", p);
			ret = FALSE;
		}
		else if (S_ISDIR(st.st_mode))
		{
			ret = drive_file_remove_dir(p);
		}
		else if (unlink(p) < 0)
		{
			DEBUG_WARN("unlink %s failed.", p);
			ret = FALSE;
		}
		else
		{
			ret = TRUE;
		}
		
		free(p);

		if (!ret)
			break;

		pdirent = readdir(dir);
	}

	closedir(dir);

	if (ret)
	{
		if (rmdir(path) < 0)
		{
			DEBUG_WARN("rmdir %s failed.", path);
			ret = FALSE;
		}
	}

	return ret;
}

static void drive_file_set_fullpath(DRIVE_FILE* file, char* fullpath)
{
	free(file->fullpath);
	file->fullpath = fullpath;
	file->filename = strrchr(file->fullpath, '/');

	if (file->filename == NULL)
		file->filename = file->fullpath;
	else
		file->filename += 1;
}

static BOOL drive_file_init(DRIVE_FILE* file, UINT32 DesiredAccess, UINT32 CreateDisposition, UINT32 CreateOptions)
{
	struct STAT st;
	BOOL exists;
#ifdef WIN32
        const static int mode = _S_IREAD | _S_IWRITE ;
#else
        const static int mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
	BOOL largeFile = FALSE;
#endif
	int oflag = 0;

	if (STAT(file->fullpath, &st) == 0)
	{
		file->is_dir = (S_ISDIR(st.st_mode) ? TRUE : FALSE);
#ifndef WIN32
		if (st.st_size > (unsigned long) 0x07FFFFFFF)
			largeFile = TRUE;
#endif
		exists = TRUE;
	}
	else
	{
		file->is_dir = ((CreateOptions & FILE_DIRECTORY_FILE) ? TRUE : FALSE);
		if (file->is_dir)
		{
			/* Should only create the directory if the disposition allows for it */
			if ((CreateDisposition == FILE_OPEN_IF) || (CreateDisposition == FILE_CREATE))
			{
				if (mkdir(file->fullpath, mode) != 0)
				{
					file->err = errno;
					return TRUE;
				}
			}
		}
		exists = FALSE;
	}

	if (file->is_dir)
	{
		file->dir = opendir(file->fullpath);

		if (file->dir == NULL)
		{
			file->err = errno;
			return TRUE;
		}
	}
	else
	{
		switch (CreateDisposition)
		{
			case FILE_SUPERSEDE:
				oflag = O_TRUNC | O_CREAT;
				break;
			case FILE_OPEN:
				break;
			case FILE_CREATE:
				oflag = O_CREAT | O_EXCL;
				break;
			case FILE_OPEN_IF:
				oflag = O_CREAT;
				break;
			case FILE_OVERWRITE:
				oflag = O_TRUNC;
				break;
			case FILE_OVERWRITE_IF:
				oflag = O_TRUNC | O_CREAT;
				break;
			default:
				break;
		}

		if ((CreateOptions & FILE_DELETE_ON_CLOSE) && (DesiredAccess & DELETE))
		{
			file->delete_pending = TRUE;
		}

		if ((DesiredAccess & GENERIC_ALL)
			|| (DesiredAccess & GENERIC_WRITE)
			|| (DesiredAccess & FILE_WRITE_DATA)
			|| (DesiredAccess & FILE_APPEND_DATA))
		{
			oflag |= O_RDWR;
		}
		else
		{
			oflag |= O_RDONLY;
		}
#ifndef WIN32
		if (largeFile)
		{
		    oflag |= O_LARGEFILE;
		}
#endif
		file->fd = OPEN(file->fullpath, oflag, mode);

		if (file->fd == -1)
		{
			file->err = errno;
			return TRUE;
		}
	}

	return TRUE;
}

DRIVE_FILE* drive_file_new(const char* base_path, const char* path, UINT32 id,
	UINT32 DesiredAccess, UINT32 CreateDisposition, UINT32 CreateOptions)
{
	DRIVE_FILE* file;

	file = (DRIVE_FILE*) malloc(sizeof(DRIVE_FILE));
	ZeroMemory(file, sizeof(DRIVE_FILE));

	file->id = id;
	file->basepath = (char*) base_path;
	drive_file_set_fullpath(file, drive_file_combine_fullpath(base_path, path));
	file->fd = -1;

	if (!drive_file_init(file, DesiredAccess, CreateDisposition, CreateOptions))
	{
		drive_file_free(file);
		return NULL;
	}

	return file;
}

void drive_file_free(DRIVE_FILE* file)
{
	if (file->fd != -1)
		close(file->fd);

	if (file->dir != NULL)
		closedir(file->dir);

	if (file->delete_pending)
	{
		if (file->is_dir)
			drive_file_remove_dir(file->fullpath);
		else
			unlink(file->fullpath);
	}

	free(file->pattern);
	free(file->fullpath);
	free(file);
}

BOOL drive_file_seek(DRIVE_FILE* file, UINT64 Offset)
{
	if (file->is_dir || file->fd == -1)
		return FALSE;

	if (LSEEK(file->fd, Offset, SEEK_SET) == (off_t)-1)
		return FALSE;

	return TRUE;
}

BOOL drive_file_read(DRIVE_FILE* file, BYTE* buffer, UINT32* Length)
{
	ssize_t r;

	if (file->is_dir || file->fd == -1)
		return FALSE;

	r = read(file->fd, buffer, *Length);

	if (r < 0)
		return FALSE;

	*Length = (UINT32) r;

	return TRUE;
}

BOOL drive_file_write(DRIVE_FILE* file, BYTE* buffer, UINT32 Length)
{
	ssize_t r;

	if (file->is_dir || file->fd == -1)
		return FALSE;

	while (Length > 0)
	{
		r = write(file->fd, buffer, Length);

		if (r == -1)
			return FALSE;

		Length -= r;
		buffer += r;
	}

	return TRUE;
}

BOOL drive_file_query_information(DRIVE_FILE* file, UINT32 FsInformationClass, wStream* output)
{
	struct STAT st;

	if (STAT(file->fullpath, &st) != 0)
	{
		stream_write_UINT32(output, 0); /* Length */
		return FALSE;
	}

	switch (FsInformationClass)
	{
		case FileBasicInformation:
			/* http://msdn.microsoft.com/en-us/library/cc232094.aspx */
			stream_write_UINT32(output, 36); /* Length */
			stream_check_size(output, 36);
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_mtime)); /* CreationTime */
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_atime)); /* LastAccessTime */
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_mtime)); /* LastWriteTime */
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_ctime)); /* ChangeTime */
			stream_write_UINT32(output, FILE_ATTR_SYSTEM_TO_RDP(file, st)); /* FileAttributes */
			/* Reserved(4), MUST NOT be added! */
			break;

		case FileStandardInformation:
			/*  http://msdn.microsoft.com/en-us/library/cc232088.aspx */
			stream_write_UINT32(output, 22); /* Length */
			stream_check_size(output, 22);
			stream_write_UINT64(output, st.st_size); /* AllocationSize */
			stream_write_UINT64(output, st.st_size); /* EndOfFile */
			stream_write_UINT32(output, st.st_nlink); /* NumberOfLinks */
			stream_write_BYTE(output, file->delete_pending ? 1 : 0); /* DeletePending */
			stream_write_BYTE(output, file->is_dir ? 1 : 0); /* Directory */
			/* Reserved(2), MUST NOT be added! */
			break;

		case FileAttributeTagInformation:
			/* http://msdn.microsoft.com/en-us/library/cc232093.aspx */
			stream_write_UINT32(output, 8); /* Length */
			stream_check_size(output, 8);
			stream_write_UINT32(output, FILE_ATTR_SYSTEM_TO_RDP(file, st)); /* FileAttributes */
			stream_write_UINT32(output, 0); /* ReparseTag */
			break;

		default:
			stream_write_UINT32(output, 0); /* Length */
			DEBUG_WARN("invalid FsInformationClass %d", FsInformationClass);
			return FALSE;
	}
	return TRUE;
}

BOOL drive_file_set_information(DRIVE_FILE* file, UINT32 FsInformationClass, UINT32 Length, wStream* input)
{
	char* s = NULL;
        mode_t m;
	UINT64 size;
	int status;
	char* fullpath;
	struct STAT st;
	struct timeval tv[2];
	UINT64 LastWriteTime;
	UINT32 FileAttributes;
	UINT32 FileNameLength;

	m = 0;

	switch (FsInformationClass)
	{
		case FileBasicInformation:
			/* http://msdn.microsoft.com/en-us/library/cc232094.aspx */
			Stream_Seek_UINT64(input); /* CreationTime */
			Stream_Seek_UINT64(input); /* LastAccessTime */
			stream_read_UINT64(input, LastWriteTime);
			Stream_Seek_UINT64(input); /* ChangeTime */
			stream_read_UINT32(input, FileAttributes);

			if (FSTAT(file->fd, &st) != 0)
				return FALSE;

			tv[0].tv_sec = st.st_atime;
			tv[0].tv_usec = 0;
			tv[1].tv_sec = (LastWriteTime > 0 ? FILE_TIME_RDP_TO_SYSTEM(LastWriteTime) : st.st_mtime);
			tv[1].tv_usec = 0;
#ifndef WIN32
/* TODO on win32 */                        
#ifdef ANDROID
			utimes(file->fullpath, tv);
#else
			futimes(file->fd, tv);
#endif

			if (FileAttributes > 0)
			{
				m = st.st_mode;
				if ((FileAttributes & FILE_ATTRIBUTE_READONLY) == 0)
					m |= S_IWUSR;
				else
					m &= ~S_IWUSR;
				if (m != st.st_mode)
					fchmod(file->fd, st.st_mode);
			}
#endif
                        break;

		case FileEndOfFileInformation:
			/* http://msdn.microsoft.com/en-us/library/cc232067.aspx */
		case FileAllocationInformation:
			/* http://msdn.microsoft.com/en-us/library/cc232076.aspx */
			stream_read_UINT64(input, size);
			if (ftruncate(file->fd, size) != 0)
				return FALSE;
			break;

		case FileDispositionInformation:
			/* http://msdn.microsoft.com/en-us/library/cc232098.aspx */
			/* http://msdn.microsoft.com/en-us/library/cc241371.aspx */
			if (Length)
				stream_read_BYTE(input, file->delete_pending);
			else
				file->delete_pending = 1;
			break;

		case FileRenameInformation:
			/* http://msdn.microsoft.com/en-us/library/cc232085.aspx */
			Stream_Seek_BYTE(input); /* ReplaceIfExists */
			Stream_Seek_BYTE(input); /* RootDirectory */
			stream_read_UINT32(input, FileNameLength);

			status = ConvertFromUnicode(CP_UTF8, 0, (WCHAR*) Stream_Pointer(input),
					FileNameLength / 2, &s, 0, NULL, NULL);

			if (status < 1)
				s = (char*) calloc(1, 1);

			fullpath = drive_file_combine_fullpath(file->basepath, s);
			free(s);

			/* TODO rename does not work on win32 */
                        if (rename(file->fullpath, fullpath) == 0)
			{
				DEBUG_SVC("renamed %s to %s", file->fullpath, fullpath);
				drive_file_set_fullpath(file, fullpath);
			}
			else
			{
				DEBUG_WARN("rename %s to %s failed, errno = %d", file->fullpath, fullpath, errno);
				free(fullpath);
				return FALSE;
			}

			break;

		default:
			DEBUG_WARN("invalid FsInformationClass %d", FsInformationClass);
			return FALSE;
	}

	return TRUE;
}

BOOL drive_file_query_directory(DRIVE_FILE* file, UINT32 FsInformationClass, BYTE InitialQuery,
	const char* path, wStream* output)
{
	int length;
	BOOL ret;
	WCHAR* ent_path;
	struct STAT st;
	struct dirent* ent;

	DEBUG_SVC("path %s FsInformationClass %d InitialQuery %d", path, FsInformationClass, InitialQuery);

	if (!file->dir)
	{
		stream_write_UINT32(output, 0); /* Length */
		stream_write_BYTE(output, 0); /* Padding */
		return FALSE;
	}

	if (InitialQuery != 0)
	{
		rewinddir(file->dir);
		free(file->pattern);

		if (path[0])
			file->pattern = _strdup(strrchr(path, '\\') + 1);
		else
			file->pattern = NULL;
	}

	if (file->pattern)
	{
		do
		{
			ent = readdir(file->dir);

			if (ent == NULL)
				continue;

			if (FilePatternMatchA(ent->d_name, file->pattern))
				break;

		}
		while (ent);
	}
	else
	{
		ent = readdir(file->dir);
	}

	if (ent == NULL)
	{
		DEBUG_SVC("  pattern %s not found.", file->pattern);
		stream_write_UINT32(output, 0); /* Length */
		stream_write_BYTE(output, 0); /* Padding */
		return FALSE;
	}

	memset(&st, 0, sizeof(struct STAT));
	ent_path = (WCHAR*) malloc(strlen(file->fullpath) + strlen(ent->d_name) + 2);
	sprintf((char*) ent_path, "%s/%s", file->fullpath, ent->d_name);

	if (STAT((char*) ent_path, &st) != 0)
	{
		DEBUG_WARN("stat %s failed. errno = %d", (char*) ent_path, errno);
	}

	DEBUG_SVC("  pattern %s matched %s", file->pattern, ent_path);
	free(ent_path);
	ent_path = NULL;

	length = ConvertToUnicode(CP_UTF8, 0, ent->d_name, -1, &ent_path, 0) * 2;

	ret = TRUE;

	switch (FsInformationClass)
	{
		case FileDirectoryInformation:
			/* http://msdn.microsoft.com/en-us/library/cc232097.aspx */
			stream_write_UINT32(output, 64 + length); /* Length */
			stream_check_size(output, 64 + length);
			stream_write_UINT32(output, 0); /* NextEntryOffset */
			stream_write_UINT32(output, 0); /* FileIndex */
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_mtime)); /* CreationTime */
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_atime)); /* LastAccessTime */
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_mtime)); /* LastWriteTime */
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_ctime)); /* ChangeTime */
			stream_write_UINT64(output, st.st_size); /* EndOfFile */
			stream_write_UINT64(output, st.st_size); /* AllocationSize */
			stream_write_UINT32(output, FILE_ATTR_SYSTEM_TO_RDP(file, st)); /* FileAttributes */
			stream_write_UINT32(output, length); /* FileNameLength */
			stream_write(output, ent_path, length);
			break;

		case FileFullDirectoryInformation:
			/* http://msdn.microsoft.com/en-us/library/cc232068.aspx */
			stream_write_UINT32(output, 68 + length); /* Length */
			stream_check_size(output, 68 + length);
			stream_write_UINT32(output, 0); /* NextEntryOffset */
			stream_write_UINT32(output, 0); /* FileIndex */
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_mtime)); /* CreationTime */
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_atime)); /* LastAccessTime */
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_mtime)); /* LastWriteTime */
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_ctime)); /* ChangeTime */
			stream_write_UINT64(output, st.st_size); /* EndOfFile */
			stream_write_UINT64(output, st.st_size); /* AllocationSize */
			stream_write_UINT32(output, FILE_ATTR_SYSTEM_TO_RDP(file, st)); /* FileAttributes */
			stream_write_UINT32(output, length); /* FileNameLength */
			stream_write_UINT32(output, 0); /* EaSize */
			stream_write(output, ent_path, length);
			break;

		case FileBothDirectoryInformation:
			/* http://msdn.microsoft.com/en-us/library/cc232095.aspx */
			stream_write_UINT32(output, 93 + length); /* Length */
			stream_check_size(output, 93 + length);
			stream_write_UINT32(output, 0); /* NextEntryOffset */
			stream_write_UINT32(output, 0); /* FileIndex */
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_mtime)); /* CreationTime */
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_atime)); /* LastAccessTime */
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_mtime)); /* LastWriteTime */
			stream_write_UINT64(output, FILE_TIME_SYSTEM_TO_RDP(st.st_ctime)); /* ChangeTime */
			stream_write_UINT64(output, st.st_size); /* EndOfFile */
			stream_write_UINT64(output, st.st_size); /* AllocationSize */
			stream_write_UINT32(output, FILE_ATTR_SYSTEM_TO_RDP(file, st)); /* FileAttributes */
			stream_write_UINT32(output, length); /* FileNameLength */
			stream_write_UINT32(output, 0); /* EaSize */
			stream_write_BYTE(output, 0); /* ShortNameLength */
			/* Reserved(1), MUST NOT be added! */
			stream_write_zero(output, 24); /* ShortName */
			stream_write(output, ent_path, length);
			break;

		case FileNamesInformation:
			/* http://msdn.microsoft.com/en-us/library/cc232077.aspx */
			stream_write_UINT32(output, 12 + length); /* Length */
			stream_check_size(output, 12 + length);
			stream_write_UINT32(output, 0); /* NextEntryOffset */
			stream_write_UINT32(output, 0); /* FileIndex */
			stream_write_UINT32(output, length); /* FileNameLength */
			stream_write(output, ent_path, length);
			break;

		default:
			stream_write_UINT32(output, 0); /* Length */
			stream_write_BYTE(output, 0); /* Padding */
			DEBUG_WARN("invalid FsInformationClass %d", FsInformationClass);
			ret = FALSE;
			break;
	}

	free(ent_path);

	return ret;
}

#ifdef _WIN32
#pragma warning(pop)
#endif
