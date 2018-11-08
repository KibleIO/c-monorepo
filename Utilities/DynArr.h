#ifndef DYNARR_H_
#define DYNARR_H_

#include <iostream>
#include <mutex>
using namespace std;

class NXTElement {
	public:
		NXTElement *next;
		NXTElement *prev;
		NXTElement() { next = NULL; prev = NULL; }
};

class DynArr {
	private:
		NXTElement *head;
		NXTElement *tail;
		volatile int length;
		mutex mylock;
	public:
		DynArr() {
			length = -1;
			head = NULL;
			tail = NULL;
		}
		~DynArr() {
			NXTElement *temp;
			while(head != NULL) {
				temp = head;
				head = head->next;
				delete temp;
			}
		}
		void clearAll() {
			mylock.lock();
			NXTElement *temp;
			while(head != NULL) {
				temp = head;
				head = head->next;
				delete temp;
			}
			length = -1;
			head   = NULL;
			tail   = NULL;
			mylock.unlock();
		}
		void Add(NXTElement *element) {
			mylock.lock();
			if (head == NULL) {
				head = element;
				tail = element;
			} else {
				tail   ->next = element;
				element->prev = tail;
				tail          = element;
			}
			length++;
			mylock.unlock();
		}
		NXTElement* RemoveFromHead() {
			mylock.lock();
			NXTElement *temp = head;
			if (head != NULL) {
				head = head->next;
				temp->next = NULL;
				length--;
			}
			mylock.unlock();
			return temp;
		}
		void Remove(NXTElement *element) {
			mylock.lock();
			NXTElement *temp = head;
			while (temp != NULL && temp != element) temp = temp->next;
			if (temp != NULL) {
				if (temp->prev == NULL) {
					head = temp->next;
					if (temp->next != NULL) temp->next->prev = NULL;
					temp->next = NULL;
				} else if (temp->next == NULL) {
					tail = temp->prev;
					temp->prev->next = NULL;
					temp->prev = NULL;
				} else {
					temp->prev->next = temp->next;
					temp->next->prev = temp->prev;
					temp->prev = NULL;
					temp->next = NULL;
				}
				length--;
			}
			mylock.unlock();
		}
		NXTElement* GetElement(int index) {
			mylock.lock();
			if (index >= 0 && index <= length) {
				NXTElement *temp = head;
				while (index > 0) {
					temp = temp->next;
					index--;
				}
				mylock.unlock();
				return temp;
			} //implied else statement
			mylock.unlock();
			return NULL;
		}
		volatile int getLength() { return length + 1; };
};

#endif /* DYNARR_H_ */
