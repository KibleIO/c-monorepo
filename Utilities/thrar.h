#ifndef THRAR_H_
#define THRAR_H_

#include <mutex>
using namespace std;


template <class T>
class Bufqueue {
public:
	Bufqueue(int num, int s = 100) {
		n = s;
		l = new T*[n];
		sl = new int[n];
		ml = new mutex[n];
		for (int i = 0; i < n; i++) {
			l[i] = new T[num];
			sl[i] = -1;
			ml[i].unlock();
		}
		oi = 0;
		mi = 0;
	}

	int size() {
		return sl[oi];
	}

	T* modify(int num = -1) {
		if (num < 0) {
			ml[mi].lock();
			log_dbg("modify open");
			return l[mi];
		} else {
			sl[mi] = num;
			ml[mi++].unlock();
			mi >= n ? mi = 0 : mi = mi;
			log_dbg("modify close");
			return NULL;
		}
	}

	T* observe(bool c = true) {
		if (c) {
			if (sl[oi] == -1) {
				return NULL;
			}
			ml[oi].lock();
			log_dbg("observe open");
			return l[oi];
		} else {
			sl[oi] = -1;
			ml[oi++].unlock();
			oi >= n ? oi = 0 : oi = oi;
			log_dbg("observe close");
			return NULL;
		}
	}
	
private:
	int oi, mi;
	int n;
	T** l;
	int* sl;
	mutex* ml;
};

template <class T>
class Thrar {
	public:

		Thrar(int n) {
			a = new T[n];
			b = new T[n];
			s = 0;
		}

		int size() {
			return s;
		}

		T* modify(int n = -1) {
			if (n < 0) {
				lockb.lock();
				log_dbg("modify open");
				return b;
			} else {
				locka.lock();
				log_dbg("modify close");

				T* c;
		
				c = a;
				a = b;
				b = c;
				s = n;
		
				locka.unlock();
				lockb.unlock();
				return NULL;
			}
		}

		T* observe(bool c = true) {
			if (c) {
				locka.lock();
				log_dbg("observe open");
				return a;
			} else {
				locka.unlock();
				log_dbg("observe close");
				return NULL;
			}
		}

	private:
		T* a;
		T* b;
		int s;
		mutex locka, lockb;
};

#endif
