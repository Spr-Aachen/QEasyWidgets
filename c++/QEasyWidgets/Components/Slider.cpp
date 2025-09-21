#include "Slider.h"

#include <cmath>

#include "../Common/StyleSheet.h"


/**
 * SliderBase implementation
 */

SliderBase::SliderBase(QWidget *parent)
    : SizableWidget<QSlider>(parent) {
    init();
}

SliderBase::SliderBase(Qt::Orientation orientation, QWidget *parent)
    : SizableWidget<QSlider>(orientation, parent) {
    init();
}

void SliderBase::init() {
    StyleSheetBase::apply(this, StyleSheetBase::Slider);
}

void SliderBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}

int SliderBase::pow10(int n) {
    static const int pows[] = {1,10,100,1000,10000,100000,1000000,10000000,100000000};
    if (n < 0) return 1;
    if (n >= static_cast<int>(sizeof(pows)/sizeof(pows[0]))) return pows[sizeof(pows)/sizeof(pows[0]) - 1];
    return pows[n];
}

int SliderBase::decimalScale(double v) {
    // Determine minimal 10^k (k<=8) so that v*10^k is effectively integral
    for (int k = 0; k <= 8; ++k) {
        double scaled = v * pow10(k);
        double rounded = std::round(scaled);
        if (std::fabs(scaled - rounded) < 1e-9) {
            return pow10(k);
        }
    }
    return pow10(8);
}

int SliderBase::updateTimesForKey(int keyId, double value) {
    // keyId: 0=min,1=max,2=step,3=value
    int Time = decimalScale(value);
    // Keep the largest scale among all fields
    Time = std::max(Time, m_scaleMinimum);
    Time = std::max(Time, m_scaleMaximum);
    Time = std::max(Time, m_scaleSingleStep);
    Time = std::max(Time, m_scaleValue);

    switch (keyId) {
        case 0: m_scaleMinimum = Time; break;
        case 1: m_scaleMaximum = Time; break;
        case 2: m_scaleSingleStep = Time; break;
        case 3: m_scaleValue = Time; break;
        default: break;
    }
    return Time;
}

void SliderBase::updateValues(int Time, const double *minOpt, const double *maxOpt, const double *stepOpt, const double *valueOpt) {
    // Minimum
    double minF = (minOpt ? *minOpt : (static_cast<double>(QSlider::minimum()) / m_scaleMinimum));
    m_scaleMinimum = Time;
    int minI = static_cast<int>(std::round(minF * Time));

    // Maximum
    double maxF = (maxOpt ? *maxOpt : (static_cast<double>(QSlider::maximum()) / m_scaleMaximum));
    m_scaleMaximum = Time;
    int maxI = static_cast<int>(std::round(maxF * Time));
    QSlider::setRange(minI, maxI);

    // Single step
    double stepF = (stepOpt ? *stepOpt : (static_cast<double>(QSlider::singleStep()) / m_scaleSingleStep));
    m_scaleSingleStep = Time;
    int stepI = static_cast<int>(std::round(stepF * Time));
    QSlider::setSingleStep(stepI);

    // Value
    double valF = (valueOpt ? *valueOpt : (static_cast<double>(QSlider::value()) / m_scaleValue));
    m_scaleValue = Time;
    int valI = static_cast<int>(std::round(valF * Time));
    QSlider::setValue(valI);
}


void SliderBase::setMinimum(double value) {
    int Time = updateTimesForKey(0, value);
    updateValues(Time, &value, nullptr, nullptr, nullptr);
}

double SliderBase::minimumF() const {
    return static_cast<double>(QSlider::minimum()) / m_scaleMinimum;
}

void SliderBase::setMaximum(double value) {
    int Time = updateTimesForKey(1, value);
    updateValues(Time, nullptr, &value, nullptr, nullptr);
}

double SliderBase::maximumF() const {
    return static_cast<double>(QSlider::maximum()) / m_scaleMaximum;
}

void SliderBase::setRangeF(double min, double max) {
    setMinimum(min);
    setMaximum(max);
}

void SliderBase::setSingleStep(double step) {
    int Time = updateTimesForKey(2, step);
    updateValues(Time, nullptr, nullptr, &step, nullptr);
}

double SliderBase::singleStepF() const {
    return static_cast<double>(QSlider::singleStep()) / m_scaleSingleStep;
}

void SliderBase::setValue(double value) {
    int Time = updateTimesForKey(3, value);
    updateValues(Time, nullptr, nullptr, nullptr, &value);
}

double SliderBase::valueF() const {
    return static_cast<double>(QSlider::value()) / m_scaleValue;
}

void SliderBase::sliderChange(QAbstractSlider::SliderChange change) {
    if (change == QAbstractSlider::SliderValueChange) {
        emit valueChangedF(valueF());
    }
    SizableWidget<QSlider>::sliderChange(change);
}