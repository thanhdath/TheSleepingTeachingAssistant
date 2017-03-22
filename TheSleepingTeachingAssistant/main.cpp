#include <stdio.h>
#include <conio.h>
#include <vector>
#include <math.h>
#include <string>
#include <iostream>
#include <Windows.h>

#define NUMBER_CHAIRS 3
#define NUMBER_STUDENTS 5
using namespace std;
class Student;
class TeachingAssistant;

class Student {
private: 
	HANDLE handle;
	string name;
	string status = "";
public:
	Student(string name) {
		this->name = name;
	}
	void program(int time) {
		status = "program";
		Sleep(time);
	}
	void setHandle(HANDLE handle) {
		this->handle = handle;
	}
	void setStatus(string status) {
		this->status = status;
	}
	string getName() {
		return name;
	}
	string getStatus() {
		return status;
	}
};

class TeachingAssistant {
private:
	HANDLE handle;
	string status = "";
public:
	TeachingAssistant(){}
	void setHandle(HANDLE handle) {
		this->handle = handle;
	}
	void setStatus(string status) {
		this->status = status;
	}
	HANDLE getHandle() {
		return handle;
	}
	string getStatus() {
		return status;
	}
};

class Chairs {
	Student *students[3] = {0};
	int numbers = 0;
public:
	Chairs(){}
	void put(Student *student, int index) {
		if (students[index] != 0)
		{
			students[index] = student;	
		}
		else {
			students[index] = student;
			numbers++;
		}
	}
	void deleteAt(int index) {
		if (students[index] != 0) {
			students[index] = 0;
			numbers--;
		}
	}
	int getNumbers() {
		return numbers;
	}
	Student* getStudent(int index) {
		return students[index];
	}
};

vector<Student*> students;
TeachingAssistant *assistant;
Chairs *chairs = new Chairs();
int help_time;

HANDLE sChairWork = CreateSemaphore(NULL, 1, 1, NULL);
HANDLE sChair1 = CreateSemaphore(NULL, 1, 1, NULL);
HANDLE sChair2 = CreateSemaphore(NULL, 1, 1, NULL);
HANDLE sChair3 = CreateSemaphore(NULL, 1, 1, NULL);
HANDLE sAssistant = CreateSemaphore(NULL, 0, 1, NULL);

void printCheck() {
	int i = 0;
	while (1) {
		system("cls");
		printf("\nStudents In Chairs: ");
		for (int i = 0; i < 3; i++) {
			if (chairs->getStudent(i) != 0)
				cout << "\t" << chairs->getStudent(i)->getName();
			else
				cout << "\t";
		}
		printf("\nAssistant: %s", assistant->getStatus().c_str());

		printf("\n Status of students: ");
		for (int i = 0; i < NUMBER_STUDENTS; i++) {
			printf("   %s: %s", students[i]->getName().c_str(), students[i]->getStatus().c_str());
		}
		Sleep(10);
		i++;
	}
}

void wait(Student *student) {
	student->setStatus("wait 3");
	WaitForSingleObject(sChair3, INFINITE);
	chairs->put(student, 2);

	student->setStatus("wait 2");
	WaitForSingleObject(sChair2, INFINITE);
	chairs->put(student, 1);
	chairs->deleteAt(2);
	ReleaseSemaphore(sChair3, 1, NULL);
	
	student->setStatus("wait 1");
	WaitForSingleObject(sChair1, INFINITE);
	chairs->put(student, 0);
	chairs->deleteAt(1);
	ReleaseSemaphore(sChair2, 1, NULL);
	
	student->setStatus("wait work");
	WaitForSingleObject(sChairWork, INFINITE);
	chairs->deleteAt(0);
	ReleaseSemaphore(sChair1, 1, NULL);
}

void studentAction(Student *student) {
	while (1) {
		int program_time = (int)(rand() % 100 + 100);
		student->program(program_time);
		// seek help
		wait(student);
		if (assistant->getStatus() == "sleep") {
			ReleaseSemaphore(sAssistant, 1, NULL);
		}
		while (help_time == 0);
		student->setStatus("help");
		Sleep(help_time);
	}
}

void assistantAction(TeachingAssistant *ta) {
	while (1) {
		if (chairs->getNumbers()) {
			// 
			help_time = (int)(rand() % 100 + 100);

			ta->setStatus("help");
			Sleep(help_time);
			help_time = 0;
			ReleaseSemaphore(sChairWork, 1, NULL);
		}
		else {
			ta->setStatus("sleep");
			WaitForSingleObject(sAssistant, INFINITE);

			help_time = (int)(rand() % 100 + 100);

			ta->setStatus("help");
			Sleep(help_time);
			help_time = 0;
			ReleaseSemaphore(sChairWork, 1, NULL);
		}
	}
}

int main() {
	for (int i = 0; i < NUMBER_STUDENTS; i++) {
		students.push_back(new Student("S" + to_string(i)));
	}
	assistant = new TeachingAssistant();

	for (int i = 0; i < NUMBER_STUDENTS; i++) {
		students[i]->setHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)studentAction, students[i], 0, NULL));
	}
	assistant->setHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)assistantAction, assistant, 0, NULL));
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)printCheck, NULL, 0, NULL);

	getchar();
	return 0;
}
