#include "face_detect.h"

void draw_boxed(dl_matrix3du_t *image_matrix, box_array_t *boxes)
{
    int x, y, w, h, i;
    uint32_t color = FACE_COLOR_GREEN;
    fb_data_t fb;

    fb.width = image_matrix->w;
    fb.height = image_matrix->h;
    fb.data = image_matrix->item;
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;
    for (i = 0; i < boxes->len; i++)
    {
        // rectangle box
        x = (int)boxes->box[i].box_p[0];
        y = (int)boxes->box[i].box_p[1];
        w = (int)boxes->box[i].box_p[2] - x + 1;
        h = (int)boxes->box[i].box_p[3] - y + 1;
        fb_gfx_drawFastHLine(&fb, x, y, w, color);
        fb_gfx_drawFastHLine(&fb, x, y + h - 1, w, color);
        fb_gfx_drawFastVLine(&fb, x, y, h, color);
        fb_gfx_drawFastVLine(&fb, x + w - 1, y, h, color);
    }
}

FaceDetect::FaceDetect(){}

FaceDetect::~FaceDetect(){}

bool FaceDetect::face_detected(camera_fb_t *fb)
{
    dl_matrix3du_t *image_matrix = NULL;
    box_array_t *net_boxes = NULL;

    image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
    fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item);
    net_boxes = face_detect(image_matrix, &mtmn_config);
    if (net_boxes)
    {
        free(net_boxes->score);
        free(net_boxes->box);
        free(net_boxes->landmark);
        free(net_boxes);
        dl_matrix3du_free(image_matrix);
        esp_camera_fb_return(fb);
        fb = NULL;
        return true;
    }
    else
    {
        dl_matrix3du_free(image_matrix);
        esp_camera_fb_return(fb);
        fb = NULL;
        return false;
    }
}

bool FaceDetect::draw_face_boxes(camera_fb_t *fb)
{
    dl_matrix3du_t *image_matrix = NULL;
    box_array_t *net_boxes = NULL;

    image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
    fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item);
    net_boxes = face_detect(image_matrix, &mtmn_config);

    if (net_boxes)
    {
        draw_boxed(image_matrix, net_boxes);
        free(net_boxes->score);
        free(net_boxes->box);
        free(net_boxes->landmark);
        free(net_boxes);
        if (!fmt2jpg(image_matrix->item, fb->width * fb->height * 3, fb->width, fb->height, PIXFORMAT_RGB888, 90, &fb->buf, &fb->len))
        {
            dl_matrix3du_free(image_matrix);
            return false;
        }
        else
        {
            dl_matrix3du_free(image_matrix);
            return true;
        }
    }
    else
    {
        dl_matrix3du_free(image_matrix);
        return false;
    }
}