#include "wifi_board.h"
#include "codecs/no_audio_codec.h"
#include "display/lcd_display.h"
#include "system_reset.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "mcp_server.h"
#include "lamp_controller.h"
#include "led/single_led.h"
#include "esp32_camera.h"
#include <wifi_station.h>
#include <esp_log.h>
#include <driver/i2c_master.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <driver/spi_common.h>
#include <driver/uart.h>
#include <driver/gpio.h>
#include "xgo.h"
#include "xgo_action.h"

#if defined(LCD_TYPE_ILI9341_SERIAL)
#include "esp_lcd_ili9341.h"
#endif

#if defined(LCD_TYPE_GC9A01_SERIAL)
#include "esp_lcd_gc9a01.h"
static const gc9a01_lcd_init_cmd_t gc9107_lcd_init_cmds[] = {
    //  {cmd, { data }, data_size, delay_ms}
    {0xfe, (uint8_t[]){0x00}, 0, 0},
    {0xef, (uint8_t[]){0x00}, 0, 0},
    {0xb0, (uint8_t[]){0xc0}, 1, 0},
    {0xb1, (uint8_t[]){0x80}, 1, 0},
    {0xb2, (uint8_t[]){0x27}, 1, 0},
    {0xb3, (uint8_t[]){0x13}, 1, 0},
    {0xb6, (uint8_t[]){0x19}, 1, 0},
    {0xb7, (uint8_t[]){0x05}, 1, 0},
    {0xac, (uint8_t[]){0xc8}, 1, 0},
    {0xab, (uint8_t[]){0x0f}, 1, 0},
    {0x3a, (uint8_t[]){0x05}, 1, 0},
    {0xb4, (uint8_t[]){0x04}, 1, 0},
    {0xa8, (uint8_t[]){0x08}, 1, 0},
    {0xb8, (uint8_t[]){0x08}, 1, 0},
    {0xea, (uint8_t[]){0x02}, 1, 0},
    {0xe8, (uint8_t[]){0x2A}, 1, 0},
    {0xe9, (uint8_t[]){0x47}, 1, 0},
    {0xe7, (uint8_t[]){0x5f}, 1, 0},
    {0xc6, (uint8_t[]){0x21}, 1, 0},
    {0xc7, (uint8_t[]){0x15}, 1, 0},
    {0xf0,
    (uint8_t[]){0x1D, 0x38, 0x09, 0x4D, 0x92, 0x2F, 0x35, 0x52, 0x1E, 0x0C,
                0x04, 0x12, 0x14, 0x1f},
    14, 0},
    {0xf1,
    (uint8_t[]){0x16, 0x40, 0x1C, 0x54, 0xA9, 0x2D, 0x2E, 0x56, 0x10, 0x0D,
                0x0C, 0x1A, 0x14, 0x1E},
    14, 0},
    {0xf4, (uint8_t[]){0x00, 0x00, 0xFF}, 3, 0},
    {0xba, (uint8_t[]){0xFF, 0xFF}, 2, 0},
};
#endif
 
#define TAG "LULUESP32S3"

LV_FONT_DECLARE(font_puhui_14_1);
LV_FONT_DECLARE(font_awesome_14_1);
LV_FONT_DECLARE(font_puhui_16_4);
LV_FONT_DECLARE(font_awesome_16_4);
LV_FONT_DECLARE(font_puhui_20_4);

class LULUESP32S3 : public WifiBoard {
private:
 
    Button boot_button_;
    LcdDisplay* display_;
    Esp32Camera* camera_;

    /*
    * @brief Initialize UART2 communication
    * The baud rate is set to 1 Mbps, with no parity, 8 data bits, 1 stop bit, and hardware flow control disabled.
    * Pin assignment:
    * - TX (Transmit): GPIO3
    * - RX (Receive):  GPIO38
    */
    void InitializeUart(){
        uart_config_t uart_cfg={
            .baud_rate=1000000,                      
            .data_bits=UART_DATA_8_BITS,  
            .parity=UART_PARITY_DISABLE,  
            .stop_bits=UART_STOP_BITS_1,             
            .flow_ctrl=UART_HW_FLOWCTRL_DISABLE, 
        };
        uart_driver_install(UART_NUM_2,1024,1024,0,NULL,0); 
        uart_param_config(UART_NUM_2,&uart_cfg);
        uart_set_pin(UART_NUM_2,GPIO_NUM_3,GPIO_NUM_38,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE); 
    }

    /*
    * @brief Initialize GPIO46 as an output for laser control.
    *
    * This function configures GPIO46 to be used as a standard output pin,
    * Configuration details:
    * - Pin: GPIO46
    * - Mode: Output
    * - Pull-up: Disabled
    * - Pull-down: Disabled
    * - Interrupt: Disabled
    */
    void gpio_laser_init() {
        esp_rom_gpio_pad_select_gpio(GPIO_NUM_46);
        gpio_reset_pin(GPIO_NUM_46);

        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << GPIO_NUM_46),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&io_conf);
    }
    
    /*
    * @brief Initialize GPIO0 as an input for the BOOT button.
    * This function configures GPIO0 to serve as an input pin for detecting
    * button presses (commonly used as the BOOT or USER button).
    * Configuration details:
    * - Pin: GPIO0
    * - Mode: Input
    * - Pull-up resistor: Enabled (keeps pin HIGH when button is not pressed)
    * - Pull-down resistor: Disabled
    * - Interrupt: Disabled
    * When the button is pressed, the pin will read LOW (0).
    */
    void gpio_boot_button_init() {
        esp_rom_gpio_pad_select_gpio(GPIO_NUM_0);
        gpio_reset_pin(GPIO_NUM_0);
        
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << GPIO_NUM_0),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE, 
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ESP_ERROR_CHECK(gpio_config(&io_conf));        
        ESP_LOGI(TAG, "GPIO_NUM_0 configured as input with pull-up resistor");
    }
    
    void InitializeSpi() {
        spi_bus_config_t buscfg = {};
        buscfg.mosi_io_num = DISPLAY_MOSI_PIN;
        buscfg.miso_io_num = GPIO_NUM_NC;
        buscfg.sclk_io_num = DISPLAY_CLK_PIN;
        buscfg.quadwp_io_num = GPIO_NUM_NC;
        buscfg.quadhd_io_num = GPIO_NUM_NC;
        buscfg.max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t);
        ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO));
    }

    void InitializeLcdDisplay() {
        esp_lcd_panel_io_handle_t panel_io = nullptr;
        esp_lcd_panel_handle_t panel = nullptr;
        //LCD Screen Control IO Initialization
        ESP_LOGD(TAG, "Install panel IO");
        esp_lcd_panel_io_spi_config_t io_config = {};
        io_config.cs_gpio_num = DISPLAY_CS_PIN;
        io_config.dc_gpio_num = DISPLAY_DC_PIN;
        io_config.spi_mode = DISPLAY_SPI_MODE;
        io_config.pclk_hz = 40 * 1000 * 1000;
        io_config.trans_queue_depth = 10;
        io_config.lcd_cmd_bits = 8;
        io_config.lcd_param_bits = 8;
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI3_HOST, &io_config, &panel_io));

        //Initialize the LCD driver chip
        ESP_LOGD(TAG, "Install LCD driver");
        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = DISPLAY_RST_PIN;
        panel_config.rgb_ele_order = DISPLAY_RGB_ORDER;
        panel_config.bits_per_pixel = 16;
#if defined(LCD_TYPE_ILI9341_SERIAL)
        ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(panel_io, &panel_config, &panel));
#elif defined(LCD_TYPE_GC9A01_SERIAL)
        ESP_ERROR_CHECK(esp_lcd_new_panel_gc9a01(panel_io, &panel_config, &panel));
        gc9a01_vendor_config_t gc9107_vendor_config = {
            .init_cmds = gc9107_lcd_init_cmds,
            .init_cmds_size = sizeof(gc9107_lcd_init_cmds) / sizeof(gc9a01_lcd_init_cmd_t),
        };        
#else
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(panel_io, &panel_config, &panel));
#endif
        
        esp_lcd_panel_reset(panel);
 

        esp_lcd_panel_init(panel);
        esp_lcd_panel_invert_color(panel, DISPLAY_INVERT_COLOR);
        esp_lcd_panel_swap_xy(panel, DISPLAY_SWAP_XY);
        esp_lcd_panel_mirror(panel, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);
#ifdef  LCD_TYPE_GC9A01_SERIAL
        panel_config.vendor_config = &gc9107_vendor_config;
#endif
        display_ = new SpiLcdDisplay(panel_io, panel,
                                    DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y, DISPLAY_SWAP_XY,
                                    {
                                        .text_font = &font_puhui_20_4,
                                        .icon_font = &font_awesome_16_4,
#if CONFIG_USE_WECHAT_MESSAGE_STYLE
                                        .emoji_font = font_emoji_32_init(),
#else
                                        .emoji_font = DISPLAY_HEIGHT >= 240 ? font_emoji_64_init() : font_emoji_32_init(),
#endif
                                    });
    }

    void InitializeCamera() {
        camera_config_t config = {};
        config.pin_d0 = CAMERA_PIN_D0;
        config.pin_d1 = CAMERA_PIN_D1;
        config.pin_d2 = CAMERA_PIN_D2;
        config.pin_d3 = CAMERA_PIN_D3;
        config.pin_d4 = CAMERA_PIN_D4;
        config.pin_d5 = CAMERA_PIN_D5;
        config.pin_d6 = CAMERA_PIN_D6;
        config.pin_d7 = CAMERA_PIN_D7;
        config.pin_xclk = CAMERA_PIN_XCLK;
        config.pin_pclk = CAMERA_PIN_PCLK;
        config.pin_vsync = CAMERA_PIN_VSYNC;
        config.pin_href = CAMERA_PIN_HREF;
        config.pin_sccb_sda = CAMERA_PIN_SIOD;  
        config.pin_sccb_scl = CAMERA_PIN_SIOC;
        config.sccb_i2c_port = 0;
        config.pin_pwdn = CAMERA_PIN_PWDN;
        config.pin_reset = CAMERA_PIN_RESET;
        config.xclk_freq_hz = XCLK_FREQ_HZ;
        config.pixel_format = PIXFORMAT_RGB565;
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
        config.fb_location = CAMERA_FB_IN_PSRAM;
        config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
        camera_ = new Esp32Camera(config);
        camera_->SetHMirror(false);
    }

    
    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting && !WifiStation::GetInstance().IsConnected()) {
                ResetWifiConfiguration();
            }
            app.ToggleChatState();
        });
    }

    /*
    * @brief Set the robot dog's movement speed and duration
    * @param dog_vx   Forward speed (positive for forward, negative for backward)
    * @param dog_vyaw Rotational speed (positive for left turn, negative for right turn)
    * @param time     Duration in milliseconds
    */
    void set_dog_speed(int dog_vx, int dog_vyaw, int time)
    {        
        motor_speed = 0;
        vx = 3.0*dog_vx;
        vyaw = 3.0*dog_vyaw;
        if(time>0){
            vTaskDelay(pdMS_TO_TICKS(time));
        }
        vx = 0.0;
        vyaw = 0.0;
    }

    /*
    * @brief Control the robot to enter or exit motor calibration mode
    * @param mode Calibration mode switch:
    *              - 1: Enter calibration mode (disable all motors)
    *              - 0: Exit calibration mode (save zero position and re-enable motors)
    */
    void Calibrate(int mode){
        if(mode==1 && calibrate_mode==0){
            EnableAllMotor(0);
            calibrate_mode = 1;
        }
        if(mode==0 && calibrate_mode==1){
            WriteZeroPos();
            EnableAllMotor(1);
            calibrate_mode = 0;            
        }
    }
    
    /* Laser sword switch control pin
    * @brief Enumeration for GPIO control modes
    * Defines three GPIO output control states:
    *              - Off (0): Turn off GPIO output  
    *              - On (1): Turn on GPIO output  
    *              - Toggle (2): Toggle the GPIO state  
    */

    enum class GpioMode {
    Off = 0,
    On = 1,
    Toggle = 2
    };
    
    /*
    * @brief Control the output level of GPIO46 
    * Sets the output level of GPIO46 according to the specified @ref GpioMode:
    *              - `GpioMode::Off`→ Set to low level  
    *              - `GpioMode::On`→ Set to high level  
    *              - `GpioMode::Toggle` → Perform a toggle operation (switch off then on)
    */
    void control_gpio(GpioMode mode) {
        switch (mode) {
            case GpioMode::Off:
                gpio_set_level(GPIO_NUM_46, 0);
                break;
            case GpioMode::On:
                gpio_set_level(GPIO_NUM_46, 1);
                break;
            case GpioMode::Toggle:
                gpio_set_level(GPIO_NUM_46, 0);
                printf("Switch lighting modes\n");
                gpio_set_level(GPIO_NUM_46, 1);
                break;
        }
    }
    
    //IoT initialization, adding support for AI-visible devices
    void InitializeIot() {
        auto& mcp_server = McpServer::GetInstance();

        mcp_server.AddTool("self.dog.move", 
        "机器狗移动(vx,vyaw,time),前后移动速度vx(前正后负,0停下)和转向速度vyaw(左转正值,右转负值,0停下),time为移动时间(毫秒),time=0时持续移动,否则移动time毫秒后停止", 
        PropertyList({
            Property("dog_vx", kPropertyTypeInteger, -100, 100),
            Property("dog_vyaw", kPropertyTypeInteger, -100, 100),
            Property("time", kPropertyTypeInteger, 0, 10000),
        }), [this](const PropertyList& properties) -> ReturnValue {
            int dog_vx = properties["dog_vx"].value<int>();
            int dog_vyaw = properties["dog_vyaw"].value<int>();
            int time = properties["time"].value<int>();
            set_dog_speed(dog_vx, dog_vyaw, time);
            return true;
        });

        mcp_server.AddTool("self.dog.calibrate", 
        "标定机器狗,1为进入标定,0为退出/完成标定", 
        PropertyList({
            Property("mode", kPropertyTypeInteger, 0, 1),
        }), [this](const PropertyList& properties) -> ReturnValue {
            int mode = properties["mode"].value<int>();
            Calibrate(mode);
            return true;
        });

        mcp_server.AddTool("self.dog.Wave", 
            "执行打招呼动作", 
            PropertyList(std::vector<Property>{}),
            [this](const PropertyList& properties) -> ReturnValue {
                Action_ID = Wave_ID;  
                return true;
            });
        
        mcp_server.AddTool("self.dog.Naughty", 
            "执行撒娇动作", 
            PropertyList(std::vector<Property>{}),
            [this](const PropertyList& properties) -> ReturnValue {
                Action_ID = Naughty_ID;  
                return true;
            });

        mcp_server.AddTool("self.dog.Swing", 
            "执行前后运动动作", 
            PropertyList(std::vector<Property>{}),
            [this](const PropertyList& properties) -> ReturnValue {
                Action_ID = Swing_ID;  
                return true;
            });

        mcp_server.AddTool("self.dog.Lookup", 
            "执行祈求/抬头动作", 
            PropertyList(std::vector<Property>{}),
            [this](const PropertyList& properties) -> ReturnValue {
                Action_ID = Lookup_ID;  
                return true;
            });

        mcp_server.AddTool("self.dog.Rolling", 
            "执行左右摇摆动作", 
            PropertyList(std::vector<Property>{}),
            [this](const PropertyList& properties) -> ReturnValue {
                Action_ID = Rolling_ID;  
                return true;
            });  

        mcp_server.AddTool("self.dog.Angry", 
            "执行懊悔/生气动作", 
            PropertyList(std::vector<Property>{}),
            [this](const PropertyList& properties) -> ReturnValue {
                Action_ID = Angry_ID;  
                return true;
            });

        mcp_server.AddTool("self.dog.Swimming", 
            "执行游泳动作", 
            PropertyList(std::vector<Property>{}),
            [this](const PropertyList& properties) -> ReturnValue {
                Action_ID = Swimming_ID;  
                return true;
            });

        mcp_server.AddTool("self.dog.Pee", 
            "执行撒尿动作", 
            PropertyList(std::vector<Property>{}),
            [this](const PropertyList& properties) -> ReturnValue {
                Action_ID = Pee_ID;  
                return true;
            });

        mcp_server.AddTool("self.dog.Stretch", 
            "执行伸懒腰动作", 
            PropertyList(std::vector<Property>{}),
            [this](const PropertyList& properties) -> ReturnValue {
                Action_ID = Stretch_ID;  
                return true;
            });

        mcp_server.AddTool("self.dog.Bouncing", 
            "执行上下蹲起动作", 
            PropertyList(std::vector<Property>{}),
            [this](const PropertyList& properties) -> ReturnValue {
                Action_ID = Bouncing_ID;  
                return true;
            });

        mcp_server.AddTool("self.dog.Shaking", 
            "执行摇头晃脑动作", 
            PropertyList(std::vector<Property>{}),
            [this](const PropertyList& properties) -> ReturnValue {
                Action_ID = Shaking_ID;  
                return true;
            });
        mcp_server.AddTool("self.dog.Sit", 
            "执行坐下动作", 
            PropertyList(std::vector<Property>{}),
            [this](const PropertyList& properties) -> ReturnValue {
                Action_ID = Sit_ID;  
                return true;
            });

        mcp_server.AddTool("self.dog.Scratch", 
            "执行挠痒动作", 
            PropertyList(std::vector<Property>{}),
            [this](const PropertyList& properties) -> ReturnValue {
                Action_ID = Scratch_ID;  
                return true;
            });

        mcp_server.AddTool("self.dog.Hug", 
            "执行抱抱动作", 
            PropertyList(std::vector<Property>{}),
            [this](const PropertyList& properties) -> ReturnValue {
                Action_ID = Hug_ID;  
                return true;
            });

        mcp_server.AddTool("self.dog.Hug", 
            "执行复位动作", 
            PropertyList(std::vector<Property>{}),
            [this](const PropertyList& properties) -> ReturnValue {
                Clear_State(2); 
                return true;
            });

        mcp_server.AddTool("self.dog.action_loop", 
            "设定表演模式/动作循环,1为开始,0为停止", 
            PropertyList({
                Property("flag", kPropertyTypeInteger, 0, 1),
            }), [this](const PropertyList& properties) -> ReturnValue {
                int flag = properties["flag"].value<int>();
                set_action_loop_flag(flag);
                return true;
            });
 
        mcp_server.AddTool("self.laser.control",
            "激光剑控制: 0=关闭, 1=打开, 2=切换激光剑模式",
            PropertyList({
                Property("mode", kPropertyTypeInteger, 0, 2),
            }),
            [this](const PropertyList& properties) -> ReturnValue {
                int modeValue = properties["mode"].value<int>();

                if (modeValue < 0 || modeValue > 2) {
                    printf("错误: 无效的 mode 值: %d\n", modeValue);
                    return false;
                }

                control_gpio(static_cast<GpioMode>(modeValue));
                return true;
            }
        );  
    }

public:
    LULUESP32S3() :
        boot_button_(BOOT_BUTTON_GPIO) {
        InitializeSpi();
        InitializeLcdDisplay();
        InitializeButtons();
        InitializeIot();
        InitializeCamera();        
        InitializeUart();
        gpio_laser_init();
        InitZeroPos();
        if (DISPLAY_BACKLIGHT_PIN != GPIO_NUM_NC) {
            GetBacklight()->RestoreBrightness();
        }
        
    }

    virtual Led* GetLed() override {
        static SingleLed led(BUILTIN_LED_GPIO);
        return &led;
    }

    virtual AudioCodec* GetAudioCodec() override {
#ifdef AUDIO_I2S_METHOD_SIMPLEX
        static NoAudioCodecSimplex audio_codec(AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_SPK_GPIO_BCLK, AUDIO_I2S_SPK_GPIO_LRCK, AUDIO_I2S_SPK_GPIO_DOUT, AUDIO_I2S_MIC_GPIO_SCK, AUDIO_I2S_MIC_GPIO_WS, AUDIO_I2S_MIC_GPIO_DIN);
#else
        static NoAudioCodecDuplex audio_codec(AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_BCLK, AUDIO_I2S_GPIO_WS, AUDIO_I2S_GPIO_DOUT, AUDIO_I2S_GPIO_DIN);
#endif
        return &audio_codec;
    }

    virtual Display* GetDisplay() override {
        return display_;
    }

    virtual Backlight* GetBacklight() override {
        if (DISPLAY_BACKLIGHT_PIN != GPIO_NUM_NC) {
            static PwmBacklight backlight(DISPLAY_BACKLIGHT_PIN, DISPLAY_BACKLIGHT_OUTPUT_INVERT);
            return &backlight;
        }
        return nullptr;
    }

    virtual Camera* GetCamera() override {
        return camera_;
    }
};

DECLARE_BOARD(LULUESP32S3);
