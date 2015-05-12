#include <primitive_scheduler.h>
#include <rotary_knob_decoder.h>

#define ACTIVE     45
#define NOT_ACTIVE 44

#define USER_LED 13

#define CHANNELS 4
#define CHANNEL_1_LED 4
#define CHANNEL_1_PWM 5
#define CHANNEL_2_LED 4
#define CHANNEL_2_PWM 5
#define CHANNEL_3_LED 4
#define CHANNEL_3_PWM 5
#define CHANNEL_4_LED 4
#define CHANNEL_4_PWM 5

typedef struct {
  unsigned char pwm;
  unsigned char led;
} channel_type;

channel_type channel_pins[CHANNELS] = {CHANNEL_1_PWM, CHANNEL_1_LED,
                                       CHANNEL_2_PWM, CHANNEL_2_LED,
                                       CHANNEL_3_PWM, CHANNEL_3_LED,
                                       CHANNEL_4_PWM, CHANNEL_4_LED};

channel_type channel_state[CHANNELS] = {255, 0,
                                        255, 0,
                                        255, 0,
                                        255, 0};

#define ROT_KNOB_RED    A7
#define ROT_KNOB_GREEN  A6
#define ROT_KNOB_BUTTON 12

PrimitiveScheduler schedule;

char rot_knob_button_state;
char active_channel;

char user_led_state;




void setup()
{
  int i;
  for (int i = 0; i < CHANNELS; i++)
  {
    pinMode(channel_pins[i].led, OUTPUT);
    pinMode(channel_pins[i].pwm, OUTPUT); 
  }
  
  pinMode(USER_LED, OUTPUT);
  pinMode(ROT_KNOB_BUTTON, INPUT_PULLUP);
  
  schedule.addTask(blink_channel_led, 250);
  //schedule.addTask(blink_user_led, 20);
  schedule.addTask(button_check, 100);

  rot_knob_button_state = NOT_ACTIVE;
  user_led_state = 0;
  active_channel = 3;

}

void loop()
{
  schedule.run();
}

void blink_user_led()
{
  digitalWrite(USER_LED, user_led_state);
  user_led_state = (user_led_state + 1) & 0x01;
}

void blink_channel_led()
{
  digitalWrite(channel_pins[active_channel].led, channel_state[active_channel].led);
  channel_state[active_channel].led = (channel_state[active_channel].led + 1) & 0x01;
}


void button_check()
{
  if ((digitalRead(ROT_KNOB_BUTTON) == LOW) && (rot_knob_button_state == NOT_ACTIVE))
  {
    active_channel++;
    rot_knob_button_state = ACTIVE;
  }
  else if (digitalRead(ROT_KNOB_BUTTON) == HIGH)
  {
    rot_knob_button_state = NOT_ACTIVE;
  }

}
