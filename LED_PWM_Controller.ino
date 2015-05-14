#include <primitive_scheduler.h>
#include <rotary_knob_decoder.h>
#include <SoftwareSerial.h>
#include "config.h"

#define ACTIVE     45
#define NOT_ACTIVE 44

#define MAX_PWM 255
#define MIN_PWM 0

typedef struct {
  unsigned char pwm;
  unsigned char led;
} channel_type;

channel_type channel_pins[CHANNELS] = {CHANNEL_1_PWM, CHANNEL_1_LED,
                                       CHANNEL_2_PWM, CHANNEL_2_LED,
                                       CHANNEL_3_PWM, CHANNEL_3_LED,
                                       CHANNEL_4_PWM, CHANNEL_4_LED};

channel_type channel_state[CHANNELS] = {MAX_PWM/4, 0,
                                        MAX_PWM/4, 0,
                                        MAX_PWM/4, 0,
                                        MAX_PWM/4, 0};

PrimitiveScheduler schedule;

RotaryKnobDecoder  knob(ROT_KNOB_A, 
                        ROT_KNOB_B, 
                        ROT_KNOB_BUTTON, 
                        ROT_KNOB_RED, 
                        ROT_KNOB_GREEN);

SoftwareSerial mySerial(SOFTWARE_SERIAL_RX, 
                        SOFTWARE_SERIAL_TX);

char serialPosition = 0;
char serialBuffer[64];

char rot_knob_button_state;
char active_channel;

char user_led_state;

void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600);
  
  int i;
  for (int i = 0; i < CHANNELS; i++)
  {
    pinMode(channel_pins[i].led, OUTPUT);
    pinMode(channel_pins[i].pwm, OUTPUT); 
  }
  
  pinMode(USER_LED, OUTPUT);
  
  schedule.addTask(blink_channel_led, 250);
  schedule.addTask(update_led_pwm, 125);
  schedule.addTask(button_check, 100);
  //schedule.addTask(stats_task, 2000);
  
  rot_knob_button_state = NOT_ACTIVE;
  user_led_state = 0;
  active_channel = 2;

  attachInterrupt(0, rot_knob, CHANGE);
  attachInterrupt(1, rot_knob, CHANGE);
}

/* Run the Ardiono loop function.
 */
void loop()
{
  schedule.run();
  
  if (mySerial.available())
  {
    serialBuffer[serialPosition] = mySerial.read();
    serialPosition++;
    serialBuffer[serialPosition] = 0;
    Serial.println(serialBuffer);
  }
}

/* The channel that is tied to the rotary knob should blink.
 */
void blink_channel_led()
{
  int i;
  for (i = 0; i < CHANNELS; i++)
  {
    digitalWrite(channel_pins[i].led, channel_state[i].led);
  }
  channel_state[active_channel].led = (channel_state[active_channel].led + 1) & 0x01;
}

/* Update the PWM outputs.
 */
void update_led_pwm()
{
  int i;
  for (i = 0; i < CHANNELS; i++)
  {
    analogWrite(channel_pins[i].pwm, channel_state[i].pwm);
  }
}

/* Check the state of the push button on the rotary knob. 
 */
void button_check()
{
  char button_state = knob.getButtonState();
  
  if ((button_state == LOW) && (rot_knob_button_state == NOT_ACTIVE))
  {
    channel_state[active_channel].led = 0;
    active_channel = (active_channel + 1) % CHANNELS;
    rot_knob_button_state = ACTIVE;
  }
  else if (button_state == HIGH)
  {
    rot_knob_button_state = NOT_ACTIVE;
  }
}

/* Processing for the rotary knob, this is fired by the state change of
 * either of the two rotational inputs.
 */
void rot_knob()
{
  int knob_direction = knob.read();
  
  if (knob_direction == CLOCKWISE && channel_state[active_channel].pwm < MAX_PWM)
  {
    channel_state[active_channel].pwm++;
  }
  else if (knob_direction == ANTICLOCKWISE && channel_state[active_channel].pwm > MIN_PWM)
  {
    channel_state[active_channel].pwm--;
  }
}


void stats_task()
{
  //TCCR0B = (TCCR0B & 0xF8) | 0x05;
  //Serial.print((int)TCCR0B & 0x07);
  Serial.print((int)active_channel);
  Serial.print(" : ");
  Serial.println((int)channel_state[active_channel].pwm);
  
  mySerial.println("Hello");
}
