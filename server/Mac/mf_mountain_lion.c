/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * OS X Server Event Handling
 *
 * Copyright 2012 Corey Clayton <can.of.tuna@gmail.com>
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

#include <dispatch/dispatch.h>
#include <CoreGraphics/CoreGraphics.h>

#include "mf_mountain_lion.h"

dispatch_semaphore_t region_sem;
dispatch_queue_t screen_update_q;
CGDisplayStreamRef stream;


bool clean;
CGRect dirtyRegion;


void (^streamHandler)(CGDisplayStreamFrameStatus, uint64_t, IOSurfaceRef, CGDisplayStreamUpdateRef) =  ^(CGDisplayStreamFrameStatus status, uint64_t displayTime, IOSurfaceRef frameSurface, CGDisplayStreamUpdateRef updateRef)
{
    /*
    if(displayTime - last_time < 500000000)
        return;
    
    last_time = displayTime;
     */
    
    /*
    printf("\tstatus: ");
    switch(status)
    {
        case kCGDisplayStreamFrameStatusFrameComplete:
            printf("Complete\n");
            break;
            
        case kCGDisplayStreamFrameStatusFrameIdle:
            printf("Idle\n");
            break;
            
        case kCGDisplayStreamFrameStatusFrameBlank:
            printf("Blank\n");
            break;
            
        case kCGDisplayStreamFrameStatusStopped:
            printf("Stopped\n");
            break;
    }
    
    printf("\ttime: %lld\n", displayTime);
    */
    
    
    const CGRect * rects;
    
    size_t num_rects;
    
    rects = CGDisplayStreamUpdateGetRects(updateRef, kCGDisplayStreamUpdateDirtyRects, &num_rects);
    
    //printf("\trectangles: %zd\n", num_rects);
    
    dispatch_semaphore_wait(region_sem, DISPATCH_TIME_FOREVER);
    
    if(clean == TRUE)
        dirtyRegion = *rects;
    
    for (size_t i = 0; i < num_rects; i++)
    {
        /*
        printf("\t\t(%f,%f),(%f,%f)\n\n",
               (rects+i)->origin.x,
               (rects+i)->origin.y,
               (rects+i)->origin.x + (rects+i)->size.width,
               (rects+i)->origin.y + (rects+i)->size.height);
         */
        
        dirtyRegion = CGRectUnion(dirtyRegion, *(rects+i));
    }
    
    /*
    printf("\t\tUnion: (%f,%f),(%f,%f)\n\n",
           uRect.origin.x,
           uRect.origin.y,
           uRect.origin.x + uRect.size.width,
           uRect.origin.y + uRect.size.height);
    */
    
    clean = FALSE;
    
    dispatch_semaphore_signal(region_sem);
        
};

int mf_mlion_screen_updates_init()
{
    printf("mf_mlion_screen_updates_init()\n");
    CGDirectDisplayID display_id;
    
    display_id = CGMainDisplayID();
    
    screen_update_q = dispatch_queue_create("mfreerdp.server.screenUpdate", NULL);
    
    region_sem = dispatch_semaphore_create(1);
 
    CGDisplayModeRef mode = CGDisplayCopyDisplayMode(display_id);
    
    size_t pixelWidth = CGDisplayModeGetPixelWidth(mode);
    size_t pixelHeight = CGDisplayModeGetPixelHeight(mode);
    
    CGDisplayModeRelease(mode);

    stream = CGDisplayStreamCreateWithDispatchQueue(display_id,
                                                    pixelWidth,
                                                    pixelHeight,
                                                    'BGRA',
                                                    NULL,
                                                    screen_update_q,
                                                    streamHandler);
    
    clean = TRUE;

    return 0;
    
}

int mf_mlion_start_getting_screen_updates()
{
    CGDisplayStreamStart(stream);
    
    return 0;

}
int mf_mlion_stop_getting_screen_updates()
{
    CGDisplayStreamStop(stream);
    
    return 0;
}

int mf_mlion_get_dirty_region(RFX_RECT* invalid)
{
    //it may be faster to copy the cgrect and then convert....
    
    dispatch_semaphore_wait(region_sem, DISPATCH_TIME_FOREVER);

    invalid->x = dirtyRegion.origin.x;
    invalid->y = dirtyRegion.origin.y;
    invalid->height = dirtyRegion.size.height;
    invalid->width = dirtyRegion.size.width;
    
    dispatch_semaphore_signal(region_sem);
    
    return 0;
}

int mf_mlion_clear_dirty_region()
{
    dispatch_semaphore_wait(region_sem, DISPATCH_TIME_FOREVER);

    clean = TRUE;
    dirtyRegion.size.width = 0;
    dirtyRegion.size.height = 0;
    
    dispatch_semaphore_signal(region_sem);
    
    return 0;
}