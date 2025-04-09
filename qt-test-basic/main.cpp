#include <QCoreApplication>
#include <QScxmlStateMachine>
#include <QDebug>

#include <iostream>
#include "build/DialogSkillAction.h"

#include <thread>
#include <chrono>

void myFunction(DialogSkillAction *machine) {
  // int num_machine_states = 3;

  // for (int i = 0; i < num_machine_states; ++i) {
  while (true) {
    std::cout << "Thread is running...\n";

    int internal_counter = 0;

    while (internal_counter < 50) {
      qDebug() << machine->activeStateNames();
      ++internal_counter;
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    // int case_index = i % num_machine_states;

    QVariantMap data;
    QString eventName;

    // switch (case_index) {
    //   case 0:
    //     std::cout << "Case 0: CMD_TICK\n";
    //     eventName = "CMD_TICK";
    //     data.insert("topic", "miao");
    //     break;
    //   case 1:
    //     std::cout << "Case 1: DialogComponent.waitForInteraction.Call\n";
    //     eventName = "DialogComponent.waitForInteraction.Return";
    //     data.insert("topic", "bau");
    //     break;
    //   default:
    //     std::cout << "Invalid case index: " << case_index << std::endl;
    //     continue;
    // }

    std::string menu = "Select a menu option:\n";
    menu += "1. CMD_TICK\n";
    menu += "2. Exit\n";
    menu += "Enter your choice: ";
    int choice;
    std::cout << menu;
    std::cin >> choice;
    switch (choice) {
      case 1:
        std::cout << "Case 1: CMD_TICK\n";
        eventName = "CMD_TICK";
        data.insert("topic", "miao");
        machine->submitEvent(eventName, data);
        break;
      case 2:
        std::cout << "Exiting...\n";
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
      default:
        std::cout << "Invalid choice. Please try again.\n";
        continue;
    }

  }

}

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  DialogSkillAction stateMachine;

  stateMachine.connectToEvent("DialogComponent.waitForInteraction.Call", [&](const QScxmlEvent &){
    QVariantMap data;
    data.insert("result", "SUCCESS");
    data.insert("interaction", "Helloo mamacita");
    data.insert("topic", "Am I in waitForInteractResult state?");
    stateMachine.submitEvent("DialogComponent.waitForInteraction.Return", data);
  });

  stateMachine.connectToEvent("SchedulerComponent.GetCurrentLanguage.Call", [&](const QScxmlEvent &){
    QVariantMap data;
    data.insert("result", "SUCCESS");
    data.insert("language", "eng");
    stateMachine.submitEvent("SchedulerComponent.GetCurrentLanguage.Return", data);
  });

  stateMachine.connectToEvent("DialogComponent.SetLanguage.Call", [&](const QScxmlEvent &){
    QVariantMap data;
    data.insert("result", "SUCCESS");
    stateMachine.submitEvent("DialogComponent.SetLanguage.Return", data);
  });

  stateMachine.connectToEvent("DialogComponent.checkDuplicate.Call", [&](const QScxmlEvent &){
    QVariantMap data;
    data.insert("result", "SUCCESS");
    data.insert("isDuplicate", false);
    stateMachine.submitEvent("DialogComponent.checkDuplicate.Return", data);
  });

  stateMachine.connectToEvent("DialogComponent.shortenAndSpeak.Call", [&](const QScxmlEvent &){
    QVariantMap data;
    data.insert("result", "SUCCESS");
    data.insert("result", "Helloo again papa");
    stateMachine.submitEvent("DialogComponent.shortenAndSpeak.Return", data);
  });

  stateMachine.connectToEvent("DialogComponent.interpret.Call", [&](const QScxmlEvent &){
    QVariantMap data;
    data.insert("result", "SUCCESS");
    data.insert("isQuestion", true);
    stateMachine.submitEvent("DialogComponent.interpret.Return", data);
  });

  stateMachine.connectToEvent("SchedulerComponent.answerAndSpeak.Call", [&](const QScxmlEvent &){
    QVariantMap data;
    data.insert("result", "SUCCESS");
    data.insert("reply", "Helloo papacito");
    stateMachine.submitEvent("SchedulerComponent.answerAndSpeak.Return", data);
  });

  stateMachine.connectToEvent("DialogComponent.GetState.Call", [&](const QScxmlEvent &){
    QVariantMap data;
    data.insert("result", "SUCCESS");
    data.insert("state", "STATUS_DONE");
    stateMachine.submitEvent("DialogComponent.GetState.Return", data);
  });

  stateMachine.connectToEvent("DialogComponent.EnableDialog.Call", [&](const QScxmlEvent &){
    QVariantMap data;
    data.insert("result", "SUCCESS");
    stateMachine.submitEvent("DialogComponent.EnableDialog.Return", data);
  });

  stateMachine.start();

  if (!stateMachine.isInitialized()) {
    qWarning() << "State machine failed to initialize!";
    return -1;
  }

  std::thread t1(myFunction, &stateMachine);

  int ret = app.exec();
  t1.join();
  return ret;
}
