// #pragma once
#ifndef FlashCalculation_h
#define FlashCalculation_h

class FlashCalculation {
public:
    FlashCalculation();
    void init();
    void generateProblem();
    bool checkAnswer();
    inline int getNumber(int index) const;
    void update();
    void setState(int num);
    inline int getState() const {return state;}
    void setDifficult(int num);
    inline int getDifficult() {return difficulty;};
    void setSpeed(int num);
    inline int getSpeed() {return speedLevel;};
    inline int getShowNumber() {return currentShowNumber;}
    inline int getUserAnswer(int index) {
      return answer[index];
    };
    void setUserAnswer(int index, int num) {
        if(answer[index] + num == 10){
          answer[index] = 0;
        }else if(answer[index] + num == -1){
          answer[index] = 9;
        }else{
          answer[index] = answer[index] + num;
        }
    }
    void setPluseMainuse() {
      if(answer[3] == 0){
        answer[3] = -1;
      }else{
        answer[3] = 0;
      }
    }


private:
    static const int PROBLEM_SIZE = 5;
    int numbers[PROBLEM_SIZE];
    int correctAnswer;
    int answer[4] = {0};
    int state;
    int difficulty;
    int speedLevel;
    unsigned long currentTime;
    unsigned long lastShowTime;
    int nextShowIndex;
    int currentShowNumber;
};

#endif