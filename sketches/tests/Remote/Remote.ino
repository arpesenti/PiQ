#include <Remote.h>
#include <IRremote.h>
#include <Pins.h>

Remote remote;
char previousStrategy;

void setup() {
  remote.init();
  previousStrategy = remote.strategy();
  Serial.begin(9600);
  Serial.println("end of init");
}

void loop() {
  remote.update();
  if (remote.strategy() != previousStrategy) {
    Serial.println("New strategy: ");
    Serial.println(remote.strategy());
    previousStrategy = remote.strategy();
  }
}
