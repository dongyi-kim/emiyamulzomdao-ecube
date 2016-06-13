//
// Created by root on 16. 6. 13.
//

#ifndef SYSPROG_CAMERA_H
#define SYSPROG_CAMERA_H
#include<string>

using namespace std;

#define CHECK(return_value)                                          \
    if (return_value < 0) {                                          \
        printf("%s::%d fail. errno: %s, m_camera_id = %d\n",             \
             __func__, __LINE__, strerror(errno), 0);      \
        return -1;                                                   \
    }

#define CHECK_PTR(return_value)                                      \
    if (return_value < 0) {                                          \
        printf("%s::%d fail, errno: %s, m_camera_id = %d\n",             \
             __func__,__LINE__, strerror(errno), 0);       \
        return NULL;                                                 \
    }


#define CAMERA_PREVIEW_WIDTH       640
#define CAMERA_PREVIEW_HEIGHT      480
#define MAX_BUFFERS     8

#define MAX_PLANES      (1)
#define V4L2_BUF_TYPE				V4L2_BUF_TYPE_VIDEO_CAPTURE

#define PREVIEW_NUM_PLANE (1)


#define V4L2_MEMORY_TYPE V4L2_MEMORY_MMAP

#define CAMERA_DEV_NAME   "/dev/video0"
#define PREVIEW_MODE 1

enum scenario_mode {
    IS_MODE_PREVIEW_STILL,
    IS_MODE_PREVIEW_VIDEO,
    IS_MODE_CAPTURE_STILL,
    IS_MODE_CAPTURE_VIDEO,
    IS_MODE_MAX
};


namespace camera{
    int CreateCamera(int index);	// create camera.
    void DestroyCamera();
    void save(const char* file_name);
}
#endif //SYSPROG_CAMERA_H

