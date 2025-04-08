#include <QCoreApplication>
#include <QScxmlStateMachine>
#include <QDebug>

#include <iostream>
// #include "MyDataModel.h"
#include "build/DialogSkillAction.h"

#include <thread>
#include <chrono>

void myFunction(DialogSkillAction *machine) {
  QMetaObject::invokeMethod(machine, [machine]() {
    machine->connectToEvent("DialogComponent.waitForInteraction.Call", [machine]([[maybe_unused]]const QScxmlEvent & event){
      QVariantMap data;
      data.insert("result", "SUCCESS");
      data.insert("interaction", "Helloo mamacita");
      data.insert("topic", "Am I in waitForInteractResult state?");
      machine->submitEvent("DialogComponent.waitForInteraction.Return", data);     
    });

    machine->connectToEvent("SchedulerComponent.GetCurrentLanguage.Call", [machine]([[maybe_unused]]const QScxmlEvent & event){
      QVariantMap data;
      data.insert("result", "SUCCESS");
      machine->submitEvent("SchedulerComponent.GetCurrentLanguage.Return", data);     
    });

    machine->connectToEvent("DialogComponent.checkDuplicate.Call", [machine]([[maybe_unused]]const QScxmlEvent & event){
      QVariantMap data;
      data.insert("result", "SUCCESS");
      data.insert("language", "eng");
      machine->submitEvent("DialogComponent.checkDuplicate.Return", data);     
    });
  }, Qt::QueuedConnection);

  int count = 2;
  int num_machine_states = 2;

  for (int i = 0; i < count; ++i) {
    std::cout << "Thread is running...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    int case_index = i % num_machine_states;

    QVariantMap data;
    QString eventName;

    switch (case_index) {
      case 0:
        std::cout << "Case 0: CMD_TICK\n";
        eventName = "CMD_TICK";
        data.insert("topic", "Am I in waitForInteract state?");
        break;
      case 1:
        std::cout << "Case 1: DialogComponent.waitForInteraction.Call\n";
        eventName = "DialogComponent.waitForInteraction.Return";
        data.insert("topic", "Am I in checkDuplicate state?");
        break;
      default:
        std::cout << "Invalid case index: " << case_index << std::endl;
        continue;
    }

    QMetaObject::invokeMethod(machine, [machine, eventName, data]() {
      machine->submitEvent(eventName, data);
    }, Qt::QueuedConnection);
  }

  // Quit the app safely
  QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}


int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  // MyDataModel dataModel;
  DialogSkillAction stateMachine;
  // stateMachine.setDataModel(&dataModel);
  stateMachine.start();

  if (!stateMachine.isInitialized()) {
    qWarning() << "State machine failed to initialize!";
  }

  std::thread t1(myFunction, &stateMachine);

  int ret=app.exec();

  t1.join();

  return ret;
  
}
