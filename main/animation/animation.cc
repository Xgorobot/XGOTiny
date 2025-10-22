#include "animation.h"
#include "lvgl.h"
#include "board.h"
#include "display.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <type_traits>

extern const lv_image_dsc_t embarrassed1;
extern const lv_image_dsc_t embarrassed2;
extern const lv_image_dsc_t embarrassed3;
extern const lv_image_dsc_t embarrassed4;
extern const lv_image_dsc_t embarrassed5;

extern const lv_image_dsc_t fire1;
extern const lv_image_dsc_t fire2;
extern const lv_image_dsc_t fire3;
extern const lv_image_dsc_t fire4;
extern const lv_image_dsc_t fire5;

extern const lv_image_dsc_t cool1;
extern const lv_image_dsc_t cool2;
extern const lv_image_dsc_t cool3;
extern const lv_image_dsc_t cool4;
extern const lv_image_dsc_t cool5;

extern const lv_image_dsc_t normal1;
extern const lv_image_dsc_t normal2;
extern const lv_image_dsc_t normal3;
extern const lv_image_dsc_t normal4;
extern const lv_image_dsc_t normal5;

extern const lv_image_dsc_t thinking1;
extern const lv_image_dsc_t thinking2;
extern const lv_image_dsc_t thinking3;
extern const lv_image_dsc_t thinking4;
extern const lv_image_dsc_t thinking5;

extern const lv_image_dsc_t sad1;
extern const lv_image_dsc_t sad2;
extern const lv_image_dsc_t sad3;
extern const lv_image_dsc_t sad4;
extern const lv_image_dsc_t sad5;

extern const lv_image_dsc_t sleepy1;
extern const lv_image_dsc_t sleepy2;
extern const lv_image_dsc_t sleepy3;
extern const lv_image_dsc_t sleepy4;
extern const lv_image_dsc_t sleepy5;

extern const lv_image_dsc_t happy1;
extern const lv_image_dsc_t happy2;
extern const lv_image_dsc_t happy3;
extern const lv_image_dsc_t happy4;
extern const lv_image_dsc_t happy5;

extern const lv_image_dsc_t laughing1;
extern const lv_image_dsc_t laughing2;
extern const lv_image_dsc_t laughing3;
extern const lv_image_dsc_t laughing4;
extern const lv_image_dsc_t laughing5;
extern const lv_image_dsc_t laughing6;

const lv_image_dsc_t *embarrass_i[5] = {
    &embarrassed1,
    &embarrassed2,
    &embarrassed3,
    &embarrassed4,
    &embarrassed5,
};

const lv_image_dsc_t *fire_i[5] = {
    &fire1,
    &fire2,
    &fire3,
    &fire4,
    &fire5,
};

const lv_image_dsc_t *inspiration_i[5] = {
    &cool1,
    &cool2,
    &cool3,
    &cool4,
    &cool5,
};

const lv_image_dsc_t *normal_i[5] = {
    &normal1,
    &normal2,
    &normal3,
    &normal4,
    &normal5,
};

const lv_image_dsc_t *question_i[5] = {
    &thinking1,
    &thinking2,
    &thinking3,
    &thinking4,
    &thinking5,
};

const lv_image_dsc_t *shy_i[5] = {
    &sad1,
    &sad2,
    &sad3,
    &sad4,
    &sad5,

};

const lv_image_dsc_t *sleep_i[5] = {
    &cool1,
    &cool2,
    &cool3,
    &cool4,
    &cool5,

};

const lv_image_dsc_t *happy_i[6] = {
    &laughing1,
    &laughing2,
    &laughing3,
    &laughing4,
    &laughing5,
    &laughing6

};

Animation_t static_normal = {
    .imges = normal_i,
    .animations = (int[]){0,1,2,3,4},
    .len = 5
};

Animation_t animation_embressed = {
    .imges = embarrass_i,
    .animations = (int[]){0,1,2,3,4},
    .len = 5
};

Animation_t animation_fire = {
    .imges = fire_i,
    .animations = (int[]){0,1,2,3,4},
    .len = 5
};

Animation_t animation_inspiration = {
    .imges = inspiration_i,
    .animations = (int[]){0,1,2,3,4},
    .len = 5
};

Animation_t animation_normal = {
    .imges = normal_i,
    .animations = (int[]){0,1,2,3,4},
    .len = 5
};

Animation_t animation_question = {
    .imges = question_i,
    .animations = (int[]){0,1,2,3,4},
    .len = 5
};

Animation_t animation_shy = {
    .imges = shy_i,
    .animations = (int[]){0,1,2,3,4},
    .len = 5
};

Animation_t animation_sleep = {
    .imges = sleep_i,
    .animations = (int[]){0,1,2,3,4},
    .len = 5
};

Animation_t animation_happy = {
    .imges = happy_i,
    .animations = (int[]){0,1,2,3,4,5},
    .len = 6
};

Animation_t *animations[] = {
    &static_normal,
    &animation_embressed,
    &animation_fire,
    &animation_inspiration,
    &animation_normal,
    &animation_question,
    &animation_shy,
    &animation_sleep,
    &animation_happy
};

static int now_animation = 0;
int pos = 0;
TaskHandle_t animation_task_handle = nullptr;

void plat_animation_task(void *arg){
    auto display = Board::GetInstance().GetDisplay();
    while(1){
        ESP_LOGD("plat_animation_task", "now_animation: %d, pos: %d", now_animation, pos);
        pos++;
        if(pos >= animations[now_animation]->len){
            pos = 0;
        }
        display->SetEmotionImg(animations[now_animation]->imges[animations[now_animation]->animations[pos]]);
        vTaskDelay(pdMS_TO_TICKS(220));
    }
    
}

void animation_set_now_animation(int animation){
    if(animation_task_handle == nullptr){
        xTaskCreate(plat_animation_task, "plat_animation_task", 2048, nullptr, 5, &animation_task_handle);
    }
    if(animation < 0 || animation >= ANIMATION_NUM){
        return;
    }
    ESP_LOGI("animation_set_now_animation", "Set now animation: %d", animation);
    now_animation = animation;
    pos = 0;
}
