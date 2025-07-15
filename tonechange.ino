const int touchSensorPin = 34;   // KY-036 analog out
const int buzzerPin = 25;        // Active buzzer pin
int threshold = 2000;            // Adjust as needed

int melody[] = {
  784, 880, 698, 784,    // G+ A+ F+ G+
  932, 880, 698, 784,    // A# A+ F+ G+
  784, 880, 698, 784,    // repeat 2
  932, 880, 698, 784,
  784, 880, 698, 784,    // repeat 3
  932, 880, 698, 784,
  784, 880, 698, 784,    // repeat 4
  932, 880, 698, 784
};

int noteDurations[] = {
  8, 8, 8, 8,
  8, 8, 8, 4,
  8, 8, 8, 8,
  8, 8, 8, 4,
  8, 8, 8, 8,
  8, 8, 8, 4,
  8, 8, 8, 8,
  8, 8, 8, 2
};

int noteIndex = 0;

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  int touchValue = analogRead(touchSensorPin);
  Serial.println(touchValue);

  if (touchValue < threshold) {
    playCurrentNote();
  } else {
    noTone(buzzerPin);  // Silence if not touching
  }

  delay(20);
}

void playCurrentNote() {
  if (noteIndex >= sizeof(melody) / sizeof(int)) {
    noteIndex = 0;  // Restart riff
    return;
  }

  int duration = 1290 / noteDurations[noteIndex];
  tone(buzzerPin, melody[noteIndex]);
  delay(duration);
  noTone(buzzerPin);
  delay(30);  // spacing

  noteIndex++;
}
