#ifndef MEDIANFILTER_H
#define MEDIANFILTER_H

class MedianFilter{
    private:
        static const int N = 11;
        double values[N];
        unsigned int curr = 0;
    public:
        void push(double input);
        double read();
};

#endif
