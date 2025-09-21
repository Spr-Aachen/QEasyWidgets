#ifndef QEASYWIDGETS_SLIDER_H
#define QEASYWIDGETS_SLIDER_H

#include <QSlider>

#include "Widget.h"


/**
 * Enhanced slider with theme support
 */
class SliderBase : public SizableWidget<QSlider> {
    Q_OBJECT
    Q_PROPERTY(int currentWidth READ currentWidth WRITE setCurrentWidth)
    Q_PROPERTY(int currentHeight READ currentHeight WRITE setCurrentHeight)

public:
    explicit SliderBase(QWidget *parent = nullptr);
    explicit SliderBase(Qt::Orientation orientation, QWidget *parent = nullptr);
    ~SliderBase() override = default;

    void clearDefaultStyleSheet();

    // Floating-point helpers
    void setMinimum(double value);
    double minimumF() const;

    void setMaximum(double value);
    double maximumF() const;

    void setRangeF(double min, double max);

    void setSingleStep(double step);
    double singleStepF() const;

    void setValue(double value);
    double valueF() const;

signals:
    void valueChanged(int value);
    void valueChangedF(double value);

private:
    void init();

    // Scaling support to emulate floating-point on integer QSlider
    int m_scaleMinimum = 1;
    int m_scaleMaximum = 1;
    int m_scaleSingleStep = 1;
    int m_scaleValue = 1;

    static int decimalScale(double v);       // returns 10^decimal_places
    static int pow10(int n);
    int updateTimesForKey(int keyId, double value);
    void updateValues(int Time, const double *minOpt, const double *maxOpt, const double *stepOpt, const double *valueOpt);

protected:
    void sliderChange(QAbstractSlider::SliderChange change) override;
};


#endif // QEASYWIDGETS_SLIDER_H