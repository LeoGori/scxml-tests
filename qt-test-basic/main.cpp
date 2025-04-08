#include <QCoreApplication>
#include <QScxmlStateMachine>
#include <QDebug>

#include <iostream>
// #include "MyDataModel.h"
#include "build/MyStateMachine.h"

#include <thread>
#include <chrono>

void myFunction(DialogSkillAction *machine) {
  int count=2;
  int num_machine_states = 2;

  machine->connectToEvent("DialogComponent.waitForInteraction.Call", [machine]([[maybe_unused]]const QScxmlEvent & event){
    QVariantMap data;
    data.insert("result", "SUCCESS");
    data.insert("interaction", "Helloo mamacita");
    data.insert("topic", "Am I in waitForInteractResult state?");
    machine->submitEvent("DialogComponent.waitForInteraction.Return", data);     
  });

  machine->connectToEvent("SchedulerComponent.getLanguage.Call", [machine]([[maybe_unused]]const QScxmlEvent & event){
    QVariantMap data;
    data.insert("result", "SUCCESS");
    machine->submitEvent("SchedulerComponent.getLanguage.Return", data);     
  });

  machine->connectToEvent("DialogComponent.checkDuplicate.Call", [machine]([[maybe_unused]]const QScxmlEvent & event){
    QVariantMap data;
    data.insert("result", "SUCCESS");
    data.insert("language", "eng");
    machine->submitEvent("DialogComponent.checkDuplicate.Return", data);     
  });

  for (int i = 0; i < count; ++i) {
    std::cout<<"Thread is running...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    int case_index = i % num_machine_states;

    switch (case_index) {
      case 0:
        machine->submitEvent("CMD_TICK", QVariantMap({
          {"topic", "Am I in waitForInteract state?"}
        }));
        break;
      case 1:
        machine->submitEvent("DialogComponent.waitForInteraction.Return", QVariantMap({
          {"topic", "Am I in checkDuplicate state?"}
        }));
        break;
      case 2:
        machine->submitEvent("", QVariantMap({
          {"topic", "Am I in waitForInteractionResult state?"}
        }));
        break;
      default:
        std::cout << "Invalid case index: " << case_index << std::endl;
        break;
    }
  }

  QCoreApplication::quit();
}

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  // MyDataModel dataModel;
  DialogSkillAction stateMachine;
  // stateMachine.setDataModel(&dataModel);

  stateMachine.start();

  std::thread t1(myFunction, &stateMachine);

  int ret=app.exec();

  t1.join();

  return ret;
  
}
