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
	int status; // 0: programming, 1: isBeingHelped, 2: waiting
public:
	Student(){}
	Student(string name) {
		this->name = name;
	}
	void program(int time) {
		status = 0;
		Sleep(time);
	}
	void setHandle(HANDLE handle) {
		this->handle = handle;
	}
	void setStatus(int status) {
		this->status = status;
	}
	HANDLE getHandle() {
		return handle;
	}
	string getName() {
		return name;
	}
	int getStatus() {
		return status;
	}
};

class TeachingAssistant {
private:
	HANDLE handle;
	int status; // 0: sleep, 1: help
public:
	TeachingAssistant(){}
	void setHandle(HANDLE handle) {
		this->handle = handle;
	}
	void sleep(HANDLE semaphore) {
		status = 0;
		WaitForSingleObject(semaphore, INFINITE);
	}
	void setStatus(int status) {
		this->status = status;
	}
	HANDLE getHandle() {
		return handle;
	}
	bool isSleeping() {
		return status == 0;
	}
};

class Chairs {
	Student *students[3] = {0};
	int numbers;
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
HANDLE SS0, SS1, SS2, SS3, SSA;

//vector<Student*> studentsInChairs;
vector<Student *> studentsInChairs;

int help_time;
Student *working = 0;

/*
void printCheck() {
	int i = 0;
	while (i < 50) {
		printf("\nStudents In Chairs: ");
		for (int i = 0; i < studentsInChairs.size(); i++) {
			printf("\t%s", studentsInChairs[i]->getName().c_str());
		}
		printf("\nAssistant: ");
		if (assistant->isSleeping()) {
			printf("Sleeping");
		}
		else printf("Working");

		printf("\n Status of students: ");
		for (int i = 0; i < 3; i++) {
			printf("\t%s:", students[i]->getName().c_str());
			if (students[i]->getStatus() == 0) {
				printf(" programming");
			}
			else if (students[i]->getStatus() == 1) {
				printf(" isBeingHelped");
			}
			else {
				printf(" waiting");
			}
		}
		printf("\n");
		Sleep(20);
		i++;
	}
}

bool hasStudentInChairs() {
	if (studentsInChairs.size() > 0)
		return true;
	return false;
}
*/
void printCheck() {
	int i = 0;
	while (1) {
		printf("\nStudents In Chairs: ");
		for (int i = 0; i < 3; i++) {
			if (chairs->getStudent(i) != 0)
				cout << "\t" << chairs->getStudent(i)->getName();
			else
				cout << "\t";
			/*
			if (chairs->getStudent(i) != NULL)
				printf("\t%s", chairs->getStudent(i)->getName().c_str());
			else
				printf("\t");
				*/
		}
		printf("\nAssistant: ");
		if (assistant->isSleeping()) {
			printf("Sleeping");
		}
		else {
			printf("Working with ");
			if (working != 0) {
				cout << working->getName();
			}
		}

		printf("\n Status of students: ");
		for (int i = 0; i < NUMBER_STUDENTS; i++) {
			printf("   %s:", students[i]->getName().c_str());
			if (students[i]->getStatus() == 0) {
				printf(" prog");
			}
			else if (students[i]->getStatus() == 1) {
				printf(" help");
			}
			else {
				printf(" wait");
			}
		}
		printf("\n");
		Sleep(50);
		i++;
	}
}

bool hasStudentInChairs() {
	if (chairs->getNumbers() > 0)
		return true;
	return false;
}

/*
int positionInChairs(Student *student) {
	for (int i = 0; i < studentsInChairs.size(); i++) {
		if (studentsInChairs[i] == student) {
			return i;
		}
	}
	return -1;
}*/

void assistantAction(TeachingAssistant *ta) {
	while (1) {
		
		if (hasStudentInChairs()) {
			// 
			help_time = (int)(rand() % 100 + 100);

			ta->setStatus(1);
			Sleep(help_time);
			help_time = 0;
			ReleaseSemaphore(SS0, 1, NULL);
		}
		else {
			ta->sleep(SSA);

			help_time = (int)(rand() % 100 + 100);

			ta->setStatus(1);
			Sleep(help_time);
			help_time = 0;
			ReleaseSemaphore(SS0, 1, NULL);
		} 
	}
}

void wait(Student *student) {
	student->setStatus(2);
	WaitForSingleObject(SS3, INFINITE);
	chairs->put(student, 2);
	WaitForSingleObject(SS2, INFINITE);
	chairs->put(student, 1);
	ReleaseSemaphore(SS3, 1, NULL);
	chairs->deleteAt(2);
	WaitForSingleObject(SS1, INFINITE);
	chairs->put(student, 0);
	ReleaseSemaphore(SS2, 1, NULL);
	chairs->deleteAt(1);
	WaitForSingleObject(SS0, INFINITE);
	ReleaseSemaphore(SS1, 1, NULL);
	chairs->deleteAt(0);
}

void studentAction(Student *student) {
	while (1) {
		int program_time = (int)(rand() % 100 + 100);
		student->program(program_time);
		// seek help
		
		wait(student);
		if (assistant->isSleeping()) {
			ReleaseSemaphore(SSA, 1, NULL);
			while (help_time == 0);
			student->setStatus(1);
			working = student;
			Sleep(help_time);
			working = 0;
		}

		
	}
}

int main() {
	for (int i = 0; i < NUMBER_STUDENTS; i++) {
		students.push_back(new Student("S" + to_string(i)));
	}
	assistant = new TeachingAssistant();

	SS0 = CreateSemaphore(NULL, 1, 1, NULL);
	SS1 = CreateSemaphore(NULL, 1, 1, NULL);
	SS2 = CreateSemaphore(NULL, 1, 1, NULL);
	SS3 = CreateSemaphore(NULL, 1, 1, NULL);
	SSA = CreateSemaphore(NULL, 0, 1, NULL);

	
	for (int i = 0; i < NUMBER_STUDENTS; i++) {
		students[i]->setHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)studentAction, students[i], 0, NULL));
	}
	assistant->setHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)assistantAction, assistant, 0, NULL));
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)printCheck, NULL, 0, NULL);



	getchar();
	return 0;
}
