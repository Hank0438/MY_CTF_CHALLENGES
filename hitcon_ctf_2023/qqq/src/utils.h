#include <QJSEngine>
#include <QElapsedTimer>

#ifndef UTILS_H
#define UTILS_H

#endif // UTILS_H

#define PADDING_SIZE 0xd8
#define CPP_TIME_TYPE 0x1
#define JS_TIME_TYPE 0x2

class QQTimer: public QObject
{
    Q_OBJECT
public:
    QQTimer() {
        timer = new QElapsedTimer();
        timeout = 0;
        jsTime = 0;
        cppTime = 0;
    }
    void start(){ timer->start(); }
    Q_INVOKABLE int elapsed(){ return timer->elapsed(); }
    void setCppTime(int val){ cppTime=val; }
    Q_INVOKABLE void setJsTime(int val){ jsTime=val; }
    void setTimeout(long long limit){ timeout = limit; }
    int getCppTime(){ return cppTime; }
    int getJsTime(){ return jsTime; }
    long long getTimeout(){ return timeout; }

    virtual ~QQTimer() {
        delete timer;
    }
private:
    long long timeout;
    QElapsedTimer* timer;
    int jsTime; // unit: seconds
    int cppTime; // unit: seconds
    char dummy[PADDING_SIZE];
};

class QQTestcase: public QObject
{
    Q_OBJECT
public:
    QQTestcase() {
        myTimer=0;
        myEngine=0;
        scriptId=0;
        used=0;
    }
    void createJsEngine(){
        if (myEngine) return;
        myEngine = new QJSEngine();
        used = 0;
    }
    void releaseJsEngine(){
        if (myEngine==0) return;
        delete myEngine;
        myEngine = 0;
    }
    void setScriptId(int id){ scriptId=id; }
    int getScriptId(){ return scriptId; }
    void setTimer(QQTimer* timer){ myTimer=timer; }
    long long getTimeout(){ return myTimer->getTimeout(); }
    void setTimeout(long long limit){ myTimer->setTimeout(limit); }
    bool checkAvailability(){ return used==0?true:false; }
    void evaluate(QString* script);
    virtual ~QQTestcase() {}
private:
    QQTimer* myTimer;
    QJSEngine* myEngine;
    int scriptId;
    int used;
    char dummy[PADDING_SIZE];

};
