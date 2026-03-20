// 快乐小车机器人 - ESP32 第一版控制程序
// 功能：基础运动控制 + LED 状态提示 + 超声波自动避障

constexpr int LEFT_PWM_PIN = 18;
constexpr int LEFT_IN1_PIN = 19;
constexpr int LEFT_IN2_PIN = 21;
constexpr int RIGHT_PWM_PIN = 5;
constexpr int RIGHT_IN1_PIN = 17;
constexpr int RIGHT_IN2_PIN = 16;
constexpr int MOTOR_STBY_PIN = 23;

constexpr int TRIG_PIN = 13;
constexpr int ECHO_PIN = 12;
constexpr int STATUS_LED_PIN = 2;

constexpr int PWM_CHANNEL_LEFT = 0;
constexpr int PWM_CHANNEL_RIGHT = 1;
constexpr int PWM_FREQ = 1000;
constexpr int PWM_RESOLUTION = 8;

constexpr int DEFAULT_SPEED = 180;
constexpr int TURN_SPEED = 170;
constexpr int SAFE_DISTANCE_CM = 25;

void setMotor(int in1Pin, int in2Pin, int pwmChannel, int speedValue, bool forward) {
  digitalWrite(in1Pin, forward ? HIGH : LOW);
  digitalWrite(in2Pin, forward ? LOW : HIGH);
  ledcWrite(pwmChannel, speedValue);
}

void stopAllMotors() {
  ledcWrite(PWM_CHANNEL_LEFT, 0);
  ledcWrite(PWM_CHANNEL_RIGHT, 0);
  digitalWrite(LEFT_IN1_PIN, LOW);
  digitalWrite(LEFT_IN2_PIN, LOW);
  digitalWrite(RIGHT_IN1_PIN, LOW);
  digitalWrite(RIGHT_IN2_PIN, LOW);
}

void moveForward(int speedValue) {
  setMotor(LEFT_IN1_PIN, LEFT_IN2_PIN, PWM_CHANNEL_LEFT, speedValue, true);
  setMotor(RIGHT_IN1_PIN, RIGHT_IN2_PIN, PWM_CHANNEL_RIGHT, speedValue, true);
}

void moveBackward(int speedValue) {
  setMotor(LEFT_IN1_PIN, LEFT_IN2_PIN, PWM_CHANNEL_LEFT, speedValue, false);
  setMotor(RIGHT_IN1_PIN, RIGHT_IN2_PIN, PWM_CHANNEL_RIGHT, speedValue, false);
}

void turnLeft(int speedValue) {
  setMotor(LEFT_IN1_PIN, LEFT_IN2_PIN, PWM_CHANNEL_LEFT, speedValue, false);
  setMotor(RIGHT_IN1_PIN, RIGHT_IN2_PIN, PWM_CHANNEL_RIGHT, speedValue, true);
}

void turnRight(int speedValue) {
  setMotor(LEFT_IN1_PIN, LEFT_IN2_PIN, PWM_CHANNEL_LEFT, speedValue, true);
  setMotor(RIGHT_IN1_PIN, RIGHT_IN2_PIN, PWM_CHANNEL_RIGHT, speedValue, false);
}

long readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) {
    return 999;
  }

  return duration / 58;
}

void setupPins() {
  pinMode(LEFT_IN1_PIN, OUTPUT);
  pinMode(LEFT_IN2_PIN, OUTPUT);
  pinMode(RIGHT_IN1_PIN, OUTPUT);
  pinMode(RIGHT_IN2_PIN, OUTPUT);
  pinMode(MOTOR_STBY_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);

  digitalWrite(MOTOR_STBY_PIN, HIGH);

  ledcSetup(PWM_CHANNEL_LEFT, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_RIGHT, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LEFT_PWM_PIN, PWM_CHANNEL_LEFT);
  ledcAttachPin(RIGHT_PWM_PIN, PWM_CHANNEL_RIGHT);
}

void setup() {
  Serial.begin(115200);
  setupPins();
  stopAllMotors();
  digitalWrite(STATUS_LED_PIN, LOW);
  Serial.println("快乐小车启动完成，进入自动避障模式。");
}

void loop() {
  long distanceCm = readDistanceCm();
  Serial.print("当前距离(cm): ");
  Serial.println(distanceCm);

  if (distanceCm > SAFE_DISTANCE_CM) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    moveForward(DEFAULT_SPEED);
    delay(80);
    return;
  }

  digitalWrite(STATUS_LED_PIN, LOW);
  stopAllMotors();
  delay(150);

  moveBackward(DEFAULT_SPEED);
  delay(250);

  turnRight(TURN_SPEED);
  delay(350);

  stopAllMotors();
  delay(100);
}
