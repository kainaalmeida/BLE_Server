#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLECharacteristic *characteristicTX; // através desse objeto iremos enviar dados para o cliente

bool deviceConected = false; //controle de dispositivo conectado

const int pin_relay = 13;

#define SERVICE_UUID "ab0828b1-198e-4351-b779-901fa0e0371g"
#define CHARACTERISTIC_UUID_RX "4ac8a682-9736-4e5d-932b-e9b31405049t"
#define CHARACTERISTIC_UUID_TX "0972EF8C-7613-4075-AD52-756F33D4DA9w"

//callback para eventos das características
class CharacteristicCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *characteristic)
  {
    //retorna ponteiro para o registrador contendo o valor atual da caracteristica
    std::string rxValue = characteristic->getValue();
    //verifica se existe dados (tamanho maior que zero)
    if (rxValue.length() > 0)
    {

      for (int i = 0; i < rxValue.length(); i++)
      {
        Serial.print(rxValue[i]);
      }
      Serial.println();
      //L1 liga o LED | L0 desliga o LED
      if (rxValue.find("L1") != -1)
      {
        digitalWrite(pin_relay, HIGH);
      }
      else if (rxValue.find("L0") != -1)
      {
        digitalWrite(pin_relay, LOW);
      }
      // B1 liga o Buzzer | B0 desliga o Buzzer
      else if (rxValue.find("B1") != -1)
      {
        // digitalWrite(BUZZER, HIGH);
      }
      else if (rxValue.find("B0") != -1)
      {
        // digitalWrite(BUZZER, LOW);
      }
    }
  } //onWrite
};

class ServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConected = true;
    Serial.println(pServer->getConnectedCount());
  }

  void onDisconnect(BLEServer *pServer)
  {
    deviceConected = false;
  }
};

void setup()
{
  Serial.begin(115200);

  pinMode(pin_relay, OUTPUT);

  //Cria o BLE device
  BLEDevice::init("SMCode-BLE"); // Nome do dispositivo bluetooth

  //Cria o BLE server
  BLEServer *server = BLEDevice::createServer();

  server->setCallbacks(new ServerCallbacks()); // seta callback do server

  //Cria o BLE service
  BLEService *service = server->createService(SERVICE_UUID);

  //Cria um BLE Characteristic para envio de dados
  characteristicTX = service->createCharacteristic(
      CHARACTERISTIC_UUID_TX,
      BLECharacteristic::PROPERTY_INDICATE);

  characteristicTX->addDescriptor(new BLE2902());

  // Cria uma Characteristic para recebimento de dados
  BLECharacteristic *characteristic = service->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE);

  characteristic->setCallbacks(new CharacteristicCallbacks());

  //Start service
  service->start();

  //Descoberta do ESP32
  server->getAdvertising()->start();

  Serial.println("Aguardando conexão de dispositivos.");
}

void loop()
{
  // put your main code here, to run repeatedly:
}