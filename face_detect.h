#include "esp_camera.h"
#include "fd_forward.h"
#include "fb_gfx.h"
#include "fd_forward.h"
#include "fr_forward.h"

#define ENROLL_CONFIRM_TIMES 5
#define FACE_ID_SAVE_NUMBER 7
#define FACE_COLOR_WHITE 0x00FFFFFF
#define FACE_COLOR_BLACK 0x00000000
#define FACE_COLOR_RED 0x000000FF
#define FACE_COLOR_GREEN 0x0000FF00
#define FACE_COLOR_BLUE 0x00FF0000
#define FACE_COLOR_YELLOW (FACE_COLOR_RED | FACE_COLOR_GREEN)
#define FACE_COLOR_CYAN (FACE_COLOR_BLUE | FACE_COLOR_GREEN)
#define FACE_COLOR_PURPLE (FACE_COLOR_BLUE | FACE_COLOR_RED)

class FaceDetect
{
private:
    static inline mtmn_config_t app_mtmn_config()
    {
        mtmn_config_t mtmn_config = {0};
        mtmn_config.type = FAST;
        mtmn_config.min_face = 60; // 80 default
        mtmn_config.pyramid = 0.707;
        mtmn_config.pyramid_times = 4;
        mtmn_config.p_threshold.score = 0.6;
        mtmn_config.p_threshold.nms = 0.7;
        mtmn_config.p_threshold.candidate_number = 20;
        mtmn_config.r_threshold.score = 0.7;
        mtmn_config.r_threshold.nms = 0.7;
        mtmn_config.r_threshold.candidate_number = 10;
        mtmn_config.o_threshold.score = 0.7;
        mtmn_config.o_threshold.nms = 0.7;
        mtmn_config.o_threshold.candidate_number = 1;
        return mtmn_config;
    }
    mtmn_config_t mtmn_config = app_mtmn_config();

public:
    FaceDetect();

    bool draw_face_boxes(camera_fb_t *fb);
    bool face_detected(camera_fb_t *fb);

    ~FaceDetect();
};