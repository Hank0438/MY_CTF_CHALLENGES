#include <QCoreApplication>
#include <QJSEngine>
#include <QTextStream>
#include <QElapsedTimer>
#include <QTimer>
#include <QThread>
#include "utils.h"

#include <io.h>
#include <fcntl.h>
#include <windows.h>

QTextStream qtin(stdin);
QTextStream qtout(stdout);

QVector<QString> scriptList = {};
QVector<int> scriptFreeList = {};
QVector<QQTestcase*> testcaseList = {};
QVector<int> testcaseFreeList = {};
QQTimer* cppTimer = 0;

void init_io() {
    // Setup I/O
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    _setmode(_fileno(stdout), O_BINARY);
    _setmode(_fileno(stdin), O_BINARY);
    _setmode(_fileno(stderr), O_BINARY);
}


void show_menu(){
    qtout << "=============================" << Qt::endl ;
    qtout << "|    Simple JS Benchmark    |" << Qt::endl ;
    qtout << "|---------------------------|" << Qt::endl ;
    qtout << "| 1. Add Script             |" << Qt::endl ;
    qtout << "| 2. Delete Script          |" << Qt::endl ;
    qtout << "| 3. Edit Script            |" << Qt::endl ;
    qtout << "| 4. View Script            |" << Qt::endl ;
    qtout << "| 5. Add Testcase           |" << Qt::endl ;
    qtout << "| 6. Delete Testcase        |" << Qt::endl ;
    qtout << "| 7. Edit Testcase          |" << Qt::endl ;
    qtout << "| 8. View Testcase          |" << Qt::endl ;
    qtout << "| 9. Run Testcase           |" << Qt::endl ;
    qtout << "| 10. Set Timer             |" << Qt::endl ;
    qtout << "| 11. Check Timer           |" << Qt::endl ;
    qtout << "| 12. Exit                  |" << Qt::endl ;
    qtout << "=============================" << Qt::endl ;
    qtout.flush();
}


void add_script(){
    QString script;
    int scriptIdx = scriptList.size();
    qtout << "Give me one-line JS script: ";
    qtout.flush();
    qtin >> script;

    if (scriptFreeList.size() > 0){
        scriptIdx = scriptFreeList.at(0);
        scriptFreeList.remove(0);
        scriptList.replace(scriptIdx, script);
    } else {
        scriptList.append(script);
    }
    qtout << "Your script index: " << scriptIdx << Qt::endl;
    qtout.flush();
}

void delete_script(){
    int scriptIdx;
    qtout << "Index: ";
    qtout.flush();
    qtin >> scriptIdx;
    if ((scriptIdx >= scriptList.size()) || ((scriptIdx < 0))){
        qtout << "Index out of bound!" << Qt::endl;
        qtout.flush();
        return;
    }
    if (scriptFreeList.indexOf(scriptIdx) >= 0){
        qtout << "Cannot access deleted script!" << Qt::endl;
        qtout.flush();
        return;
    }
    scriptFreeList.append(scriptIdx);
}

void edit_script(){
    QString script;
    int scriptIdx;
    qtout << "Index: ";
    qtout.flush();
    qtin >> scriptIdx;
    if ((scriptIdx >= scriptList.size()) || ((scriptIdx < 0))){
        qtout << "Index out of bound!" << Qt::endl;
        qtout.flush();
        return;
    }
    if (scriptFreeList.indexOf(scriptIdx) >= 0){
        qtout << "Cannot access deleted script!" << Qt::endl;
        qtout.flush();
        return;
    }
    qtout << "Give me one-line JavaScript: ";
    qtout.flush();
    qtin >> script;
    scriptList.replace(scriptIdx, script);
}

void view_script(){
    int scriptIdx;
    qtout << "Index: ";
    qtout.flush();
    qtin >> scriptIdx;
    if ((scriptIdx >= scriptList.size()) || ((scriptIdx < 0))){
        qtout << "Index out of bound!" << Qt::endl;
        qtout.flush();
        return;
    }
    if (scriptFreeList.indexOf(scriptIdx) >= 0){
        qtout << "Cannot access deleted script!" << Qt::endl;
        qtout.flush();
        return;
    }
    qtout << "-- Your Script --" << Qt::endl;
    qtout << scriptList.at(scriptIdx) << Qt::endl;
    qtout.flush();
}

// enable CPP/JS timer
void add_testcase(){
    int testcaseIdx = testcaseList.size();
    int scriptIdx;
    long long timeout;
    QQTestcase* testcase;

    qtout << "Script Index: ";
    qtout.flush();
    qtin >> scriptIdx;
    if ((scriptIdx >= scriptList.size()) || ((scriptIdx < 0))){
        qtout << "Index out of bound!" << Qt::endl;
        qtout.flush();
        return;
    }
    if (scriptFreeList.indexOf(scriptIdx) >= 0){
        qtout << "Cannot access deleted script!" << Qt::endl;
        qtout.flush();
        return;
    }
    qtout << "Set timeout: ";
    qtout.flush();
    qtin >> timeout;

    if (testcaseFreeList.size() > 0){
        testcaseIdx = testcaseFreeList.at(0);
        testcaseFreeList.remove(0);
        testcase = testcaseList.at(testcaseIdx);
    } else {
        testcase = new QQTestcase();
        testcaseList.append(testcase);
    }
    if (cppTimer==0){
        qtout << "Initialize cppTimer!" << Qt::endl;
        qtout.flush();
        cppTimer = new QQTimer();
    }
    testcase->setTimer(cppTimer);
    testcase->createJsEngine();
    testcase->setTimeout(timeout);
    testcase->setScriptId(scriptIdx);

    qtout << "Your testcase index: " << testcaseIdx << Qt::endl;
    qtout.flush();
}

void delete_testcase(){
    QQTestcase* testcase;
    int testcaseIdx;
    qtout << "Testcase Index: ";
    qtout.flush();
    qtin >> testcaseIdx;
    if ((testcaseIdx >= testcaseList.size()) || (testcaseIdx<0)){
        qtout << "Index out of bound!" << Qt::endl;
        qtout.flush();
        return;
    }
    if (testcaseFreeList.indexOf(testcaseIdx) >= 0){
        qtout << "Cannot access deleted testcase!" << Qt::endl;
        qtout.flush();
        return;
    }
    testcaseFreeList.append(testcaseIdx);
    testcase = testcaseList.at(testcaseIdx);
    testcase->releaseJsEngine();
}

void view_testcase(){
    QQTestcase* testcase;
    int testcaseIdx;
    qtout << "Testcase Index: ";
    qtout.flush();
    qtin >> testcaseIdx;
    if ((testcaseIdx >= testcaseList.size()) || (testcaseIdx<0)){
        qtout << "Index out of bound!" << Qt::endl;
        qtout.flush();
        return;
    }
    if (testcaseFreeList.indexOf(testcaseIdx) >= 0){
        qtout << "Cannot access deleted testcase!" << Qt::endl;
        qtout.flush();
        return;
    }
    testcase = testcaseList.at(testcaseIdx);

    qtout << "-- Testcase Information --" << Qt::endl;
    qtout << "Script Index: " << testcase->getScriptId() << Qt::endl;
    qtout << "Timeout: " << testcase->getTimeout() << Qt::endl;
    qtout.flush();

}

void edit_testcase(){
    QQTestcase* testcase;
    int testcaseIdx;
    int scriptIdx;
    long long timeout;

    qtout << "Testcase Index: ";
    qtout.flush();
    qtin >> testcaseIdx;
    if ((testcaseIdx >= testcaseList.size()) || (testcaseIdx<0)){
        qtout << "Index out of bound!" << Qt::endl;
        qtout.flush();
        return;
    }
    if (testcaseFreeList.indexOf(testcaseIdx) >= 0){
        qtout << "Cannot access deleted testcase!" << Qt::endl;
        qtout.flush();
        return;
    }
    testcase = testcaseList.at(testcaseIdx);

    qtout << "Script Index: ";
    qtout.flush();
    qtin >> scriptIdx;
    if ((scriptIdx >= scriptList.size()) || ((scriptIdx < 0))){
        qtout << "Index out of bound!" << Qt::endl;
        qtout.flush();
        return;
    }
    if (scriptFreeList.indexOf(scriptIdx) >= 0){
        qtout << "Cannot access deleted script!" << Qt::endl;
        qtout.flush();
        return;
    }
    testcase->setScriptId(scriptIdx);

    qtout << "Set timeout: ";
    qtout.flush();
    qtin >> timeout;
    testcase->setTimeout(timeout);
}

void set_timer(){
    long long timeout;
    qtout << "Set timeout: ";
    qtout.flush();
    qtin >> timeout;
    if (cppTimer==0){
        qtout << "Initialize cppTimer!" << Qt::endl;
        qtout.flush();
        cppTimer = new QQTimer();
    }
    cppTimer->setTimeout(timeout);
}

void check_timer(){
    if (cppTimer==0){
        qtout << "Initialize cppTimer!" << Qt::endl;
        qtout.flush();
        cppTimer = new QQTimer();
    }
    qtout << "timeout: " << cppTimer->getTimeout() << Qt::endl;
    qtout << "jsTime: " << cppTimer->getCppTime() << Qt::endl;
    qtout << "cppTime: " << cppTimer->getJsTime() << Qt::endl;
    qtout.flush();
}

// run_testcase() will launch all engine in the list to run the destinated JS script
void run_testcase(){
    QQTestcase* testcase;
    QString* currentScript;
    int testcaseIdx;
    int scriptIdx;

    qtout << "Testcase Index: ";
    qtout.flush();
    qtin >> testcaseIdx;
    if (testcaseIdx >= testcaseList.size()){
        qtout << "Index out of bound!" << Qt::endl;
        qtout.flush();
        return;
    }
    if (testcaseFreeList.indexOf(testcaseIdx) >= 0){
        qtout << "Cannot access deleted testcase!" << Qt::endl;
        qtout.flush();
        return;
    }
    testcase = testcaseList.at(testcaseIdx);

    scriptIdx = testcase->getScriptId();
    if (scriptFreeList.indexOf(scriptIdx) >= 0){
        qtout << "Cannot access deleted script!" << Qt::endl;
        qtout.flush();
        return;
    }
    currentScript = &scriptList[scriptIdx];
    testcase->evaluate(currentScript);
    qtout << "-- Testcase Result --" << Qt::endl;
    qtout << "Testcase id: " << testcaseIdx << Qt::endl;
    qtout << "Script id: " << scriptIdx << Qt::endl;
    qtout.flush();
    check_timer();
}


void QQTestcase::evaluate(QString* script){
    QJSValue jsTimer;
    QString evalScript;
    QJSValue func;
    QThread *thread = new QThread();
    QTimer* timer = new QTimer();

    long long timeout = myTimer->getTimeout();
    if (timeout > 5000){
        timeout = 5000;
    }
    if (timeout < 0){
        timeout = 0;
    }
    if (used==0) {
        myEngine->installExtensions(QJSEngine::ConsoleExtension);
        jsTimer = myEngine->newQObject(myTimer); // leak heap addr
        myEngine->globalObject().setProperty("Timer", jsTimer);
    }
    myTimer->start();
    myEngine->evaluate("var start_time = Timer.elapsed();");
    thread->start();
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, myEngine, [&]{myEngine->setInterrupted(true);}, Qt::DirectConnection);
    timer->moveToThread(thread);
    QMetaObject::invokeMethod(timer, "start", Q_ARG(int, timeout));

    evalScript = QString("((function(){%1}))").arg(*script);
    func = myEngine->evaluate(evalScript);
    if (func.isError()) {
        qtout << func.toString() << Qt::endl;
        return;
    }
    func.call();

    thread->terminate();
    myEngine->setInterrupted(false);
    myEngine->evaluate("var end_time=Timer.elapsed()");
    myEngine->evaluate("Timer.setJsTime(end_time-start_time)");
    myTimer->setCppTime(myTimer->elapsed());
    used+=1;

    delete timer;
    delete thread;
}

int main(int argc, char *argv[]){
    unsigned int choice;

    QCoreApplication a(argc, argv);
    init_io();
    while(1){
        show_menu();
        qtout << "Your choice: ";
        qtout.flush();
        qtin >> choice;
        switch(choice){
            case 1:
                add_script();
                break;
            case 2:
                delete_script();
                break;
            case 3:
                edit_script();
                break;
            case 4:
                view_script();
                break;
            case 5:
                add_testcase();
                break;
            case 6:
                delete_testcase();
                break;
            case 7:
                edit_testcase();
                break;
            case 8:
                view_testcase();
                break;
            case 9:
                run_testcase();
                break;
            case 10:
                set_timer();
                break;
            case 11:
                check_timer();
                break;
            case 12:
                return -1;
            default:
                break;
        }
    }
//    return a.exec();
    return 0;
}


