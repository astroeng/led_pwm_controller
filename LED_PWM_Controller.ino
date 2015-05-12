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
#define ROT_KNOB_A      2
#define ROT_KNOB_B      3

PrimitiveScheduler schedule;
RotaryKnobDecoder  knob;

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
  pinMode(ROT_KNOB_A, INPUT_PULLUP);
  pinMode(ROT_KNOB_B, INPUT_PULLUP);
  
  schedule.addTask(blink_channel_led, 250);
  schedule.addTask(update_led_pwm, 125);
  //schedule.addTask(blink_user_led, 20);
  schedule.addTask(button_check, 100);

  knob = RotaryKnobDecoder(ROT_KNOB_A, ROT_KNOB_B);
  
  rot_knob_button_state = NOT_ACTIVE;
  user_led_state = 0;
  active_channel = 3;

  attachInterrupt(0, rot_knob, CHANGE);
  attachInterrupt(1, rot_knob, CHANGE);

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
  int i;
  for (i = 0; i < CHANNELS; i++)
  {
    digitalWrite(channel_pins[i].led, channel_state[i].led);
  }
  channel_state[active_channel].led = (channel_state[active_channel].led + 1) & 0x01;
}


void update_led_pwm()
{
  int i;
  for (i = 0; i < CHANNELS; i++)
  {
    analogWrite(channel_pins[i].pwm, channel_state[i].pwm);
  }
}


void button_check()
{
  char button_state = digitalRead(ROT_KNOB_BUTTON);
  
  if ((button_state == LOW) && (rot_knob_button_state == NOT_ACTIVE))
  {
    channel_state[active_channel].led = 0;
    active_channel++;
    rot_knob_button_state = ACTIVE;
  }
  else if (button_state == HIGH)
  {
    rot_knob_button_state = NOT_ACTIVE;
  }

}


void rot_knob()
{
  
  int knob_direction = knob.read();
  
  if (knob_direction == CLOCKWISE)
  {
    channel_state[active_channel].pwm++;
  }
  else if (knob_direction == ANTICLOCKWISE)
  {
    channel_state[active_channel].pwm--;
  }

}
