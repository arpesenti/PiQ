#include <Remote.h>
#include <IRremote.h>

Remote remote;
char previousStrategy;

void setup() {
  remote.init();
  previousStrategy = remote.strategy();
}

void loop() {
  remote.update();
  if (remote.strategy() != previousStrategy) {
    Serial.println("New strategy: ");
    Serial.println(remote.strategy());
    previousStrategy = remote.strategy();
  }
}
