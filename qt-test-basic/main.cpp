#include <QCoreApplication>
#include <QScxmlStateMachine>
#include <QDebug>

#include <iostream>
#include "build/DialogSkillAction.h"

#include <thread>
#include <chrono>

void myFunction(DialogSkillAction *machine) {
  int num_machine_states = 3;

  for (int i = 0; i < num_machine_states; ++i) {
    std::cout << "Thread is running...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    int case_index = i % num_machine_states;

    QVariantMap data;
    QString eventName;

    switch (case_index) {
      case 0:
        std::cout << "Case 0: CMD_TICK\n";
        eventName = "CMD_TICK";
        data.insert("topic", "miao");
        break;
      case 1:
        std::cout << "Case 1: DialogComponent.waitForInteraction.Call\n";
        eventName = "DialogComponent.waitForInteraction.Return";
        data.insert("topic", "bau");
        break;
      default:
        std::cout << "Invalid case index: " << case_index << std::endl;
        continue;
    }

    machine->submitEvent(eventName, data);
  }

  QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
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

  stateMachine.connectToEvent("DialogComponent.checkDuplicate.Call", [&](const QScxmlEvent &){
    QVariantMap data;
    data.insert("result", "SUCCESS");
    data.insert("isDuplicate", false);
    stateMachine.submitEvent("DialogComponent.checkDuplicate.Return", data);
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
