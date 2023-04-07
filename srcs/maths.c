#include "../inc/ft_ping.h"

double my_pow(double base, double exponent) 
{
    double result = 1.0;
    int i;

    if (exponent > 0) {
        for (i = 0; i < exponent; i++) {
            result *= base;
        }
    } else if (exponent < 0) {
        for (i = 0; i > exponent; i--) {
            result /= base;
        }
    }

    return result;
}

double my_sqrt(double x) 
{
    double guess = 1.0;
    double old_guess;
    double diff = 1.0;
    double epsilon = 0.000001;
    
    do {
        old_guess = guess;
        guess = (guess + x / guess) / 2.0;
        diff = guess - old_guess;
        if (diff < 0) {
            diff = -diff;
        }
    } while (diff > epsilon);
    
    return guess;
}

float calc_stddev() 
{
    float variance = 0.0;

    for (int i = 0; i < g_all.packet_sent - 1; i++) {
        variance += my_pow(g_all.data[i] - (g_all.avg / (g_all.packet_sent + 1)), 2);
    }
    variance /= g_all.packet_sent;

    return my_sqrt(variance);
}