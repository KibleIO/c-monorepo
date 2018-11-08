#ifndef BUFFER_H_
#define BUFFER_H_

class Buffer {
private:
        char *Swap;
        int swapper;
public:
        char *main;
        char *back;
        int size, size_main, size_back;
        Buffer(int s) {
                size = s;
                size_main = size;
                size_back = size;
                main = new char[size];
                back = new char[size];
        }
        void out_back(char c) {
                for (int i = 0; i < size; i++) back[i] = c;
        }
        void out_main(char c) {
                for (int i = 0; i < size; i++) main[i] = c;
        }
        void out(int c) {
                out_back(c);
                out_main(c);
        }
        void swap() {
                Swap = main;
                main = back;
                back = Swap;

                swapper = size_main;
                size_main = size_back;
                size_back = swapper;
        }
};

#endif /* BUFFER_H_ */