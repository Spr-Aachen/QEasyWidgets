#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QDebug>

#include "../../QEasyWidgets/QEasyWidgets_All.h"


/**
 * @brief Main gallery window demonstrating QEasyWidgets components
 */
class ShowcaseWindow : public MainWindowBase
{
    Q_OBJECT

public:
    explicit ShowcaseWindow(QWidget *parent = nullptr)
        : MainWindowBase(parent, Qt::Window | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint, 1200, 800) {
        setWindowTitle("QEasyWidgets Showcase - C++ Component Library");
        setupUI();
    }

private:
    void setupUI() {
        // Create central widget
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setContentsMargins(20, 20, 20, 20);
        mainLayout->setSpacing(15);

        // Title
        LabelBase *titleLabel = new LabelBase("QEasyWidgets Showcase", this);
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize(24);
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // Subtitle
        LabelBase *subtitleLabel = new LabelBase("Explore beautiful Qt components with modern theming", this);
        QFont subtitleFont = subtitleLabel->font();
        subtitleFont.setPointSize(12);
        subtitleLabel->setFont(subtitleFont);
        subtitleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(subtitleLabel);

        mainLayout->addSpacing(10);

        // Create tab widget
        TabWidgetBase *tabWidget = new TabWidgetBase(this);
        mainLayout->addWidget(tabWidget);

        // Add tabs
        tabWidget->addTab(createButtonsTab(), "Buttons");
        tabWidget->addTab(createInputsTab(), "Inputs");
        tabWidget->addTab(createComponentsTab(), "Components");
        tabWidget->addTab(createAboutTab(), "About");

        setCentralWidget(centralWidget);
    }

    /**
     * @brief Creates the buttons demonstration tab
     */
    QWidget* createButtonsTab() {
        QWidget *widget = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(widget);
        layout->setContentsMargins(20, 20, 20, 20);
        layout->setSpacing(20);

        // Primary Buttons Section
        QGroupBox *primaryGroup = new QGroupBox("Primary Buttons");
        QHBoxLayout *primaryLayout = new QHBoxLayout(primaryGroup);
        primaryLayout->setSpacing(10);

        PrimaryButton *btn1 = new PrimaryButton("Save", this);
        btn1->setMinimumSize(120, 40);
        connect(btn1, &QPushButton::clicked, this, [this]() {
            showMessage("Primary Button", "Save button clicked!");
        });
        primaryLayout->addWidget(btn1);

        PrimaryButton *btn2 = new PrimaryButton("Submit", this);
        btn2->setMinimumSize(120, 40);
        connect(btn2, &QPushButton::clicked, this, [this]() {
            showMessage("Primary Button", "Submit button clicked!");
        });
        primaryLayout->addWidget(btn2);

        PrimaryButton *btn3 = new PrimaryButton("Confirm", this);
        btn3->setMinimumSize(120, 40);
        connect(btn3, &QPushButton::clicked, this, [this]() {
            showMessage("Primary Button", "Confirm button clicked!");
        });
        primaryLayout->addWidget(btn3);

        primaryLayout->addStretch();
        layout->addWidget(primaryGroup);

        // Standard Buttons Section
        QGroupBox *standardGroup = new QGroupBox("Standard Buttons");
        QHBoxLayout *standardLayout = new QHBoxLayout(standardGroup);
        standardLayout->setSpacing(10);

        ButtonBase *stdBtn1 = new ButtonBase("Open", this);
        stdBtn1->setMinimumSize(120, 40);
        connect(stdBtn1, &QPushButton::clicked, this, [this]() {
            showMessage("Standard Button", "Open button clicked!");
        });
        standardLayout->addWidget(stdBtn1);

        ButtonBase *stdBtn2 = new ButtonBase("Close", this);
        stdBtn2->setMinimumSize(120, 40);
        connect(stdBtn2, &QPushButton::clicked, this, [this]() {
            showMessage("Standard Button", "Close button clicked!");
        });
        standardLayout->addWidget(stdBtn2);

        ButtonBase *stdBtn3 = new ButtonBase("Cancel", this);
        stdBtn3->setMinimumSize(120, 40);
        connect(stdBtn3, &QPushButton::clicked, this, [this]() {
            showMessage("Standard Button", "Cancel button clicked!");
        });
        standardLayout->addWidget(stdBtn3);

        standardLayout->addStretch();
        layout->addWidget(standardGroup);

        // Transparent Buttons Section
        QGroupBox *transparentGroup = new QGroupBox("Transparent Buttons");
        QHBoxLayout *transparentLayout = new QHBoxLayout(transparentGroup);
        transparentLayout->setSpacing(10);

        TransparentButton *transBtn1 = new TransparentButton("Settings", this);
        transBtn1->setMinimumSize(120, 40);
        connect(transBtn1, &QPushButton::clicked, this, [this]() {
            showMessage("Transparent Button", "Settings button clicked!");
        });
        transparentLayout->addWidget(transBtn1);

        TransparentButton *transBtn2 = new TransparentButton("Help", this);
        transBtn2->setMinimumSize(120, 40);
        connect(transBtn2, &QPushButton::clicked, this, [this]() {
            showMessage("Transparent Button", "Help button clicked!");
        });
        transparentLayout->addWidget(transBtn2);

        TransparentButton *transBtn3 = new TransparentButton("About", this);
        transBtn3->setMinimumSize(120, 40);
        connect(transBtn3, &QPushButton::clicked, this, [this]() {
            showMessage("Transparent Button", "About button clicked!");
        });
        transparentLayout->addWidget(transBtn3);

        transparentLayout->addStretch();
        layout->addWidget(transparentGroup);

        // Disabled Buttons Section
        QGroupBox *disabledGroup = new QGroupBox("Disabled State");
        QHBoxLayout *disabledLayout = new QHBoxLayout(disabledGroup);
        disabledLayout->setSpacing(10);

        PrimaryButton *disBtn1 = new PrimaryButton("Disabled Primary", this);
        disBtn1->setMinimumSize(150, 40);
        disBtn1->setEnabled(false);
        disabledLayout->addWidget(disBtn1);

        ButtonBase *disBtn2 = new ButtonBase("Disabled Standard", this);
        disBtn2->setMinimumSize(150, 40);
        disBtn2->setEnabled(false);
        disabledLayout->addWidget(disBtn2);

        disabledLayout->addStretch();
        layout->addWidget(disabledGroup);

        layout->addStretch();
        return widget;
    }

    /**
     * @brief Creates the inputs demonstration tab
     */
    QWidget* createInputsTab() {
        QWidget *widget = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(widget);
        layout->setContentsMargins(20, 20, 20, 20);
        layout->setSpacing(20);

        // Line Edit Section
        QGroupBox *lineEditGroup = new QGroupBox("Line Edit");
        QVBoxLayout *lineEditLayout = new QVBoxLayout(lineEditGroup);
        lineEditLayout->setSpacing(15);

        // Basic line edit
        QHBoxLayout *basicLineLayout = new QHBoxLayout();
        LabelBase *basicLineLabel = new LabelBase("Basic Input:", this);
        basicLineLabel->setMinimumWidth(120);
        LineEditBase *basicLineEdit = new LineEditBase(this);
        basicLineEdit->setPlaceholderText("Enter text here...");
        basicLineEdit->setMinimumHeight(35);
        basicLineLayout->addWidget(basicLineLabel);
        basicLineLayout->addWidget(basicLineEdit);
        lineEditLayout->addLayout(basicLineLayout);

        // Line edit with clear button
        QHBoxLayout *clearLineLayout = new QHBoxLayout();
        LabelBase *clearLineLabel = new LabelBase("With Clear Button:", this);
        clearLineLabel->setMinimumWidth(120);
        LineEditBase *clearLineEdit = new LineEditBase(this);
        clearLineEdit->setPlaceholderText("Type to see clear button...");
        clearLineEdit->setClearButtonEnabled(true);
        clearLineEdit->setMinimumHeight(35);
        clearLineLayout->addWidget(clearLineLabel);
        clearLineLayout->addWidget(clearLineEdit);
        lineEditLayout->addLayout(clearLineLayout);

        // Password input
        QHBoxLayout *passwordLayout = new QHBoxLayout();
        LabelBase *passwordLabel = new LabelBase("Password:", this);
        passwordLabel->setMinimumWidth(120);
        LineEditBase *passwordEdit = new LineEditBase(this);
        passwordEdit->setPlaceholderText("Enter password...");
        passwordEdit->setEchoMode(QLineEdit::Password);
        passwordEdit->setMinimumHeight(35);
        passwordLayout->addWidget(passwordLabel);
        passwordLayout->addWidget(passwordEdit);
        lineEditLayout->addLayout(passwordLayout);

        layout->addWidget(lineEditGroup);

        // Text Edit Section
        QGroupBox *textEditGroup = new QGroupBox("Text Edit");
        QVBoxLayout *textEditLayout = new QVBoxLayout(textEditGroup);

        LabelBase *textEditLabel = new LabelBase("Multi-line Input:", this);
        textEditLayout->addWidget(textEditLabel);

        TextEditBase *textEdit = new TextEditBase(this);
        textEdit->setPlaceholderText("Enter multiple lines of text here...\n\nThis is a multi-line text editor with theme support.");
        textEdit->setMinimumHeight(150);
        textEditLayout->addWidget(textEdit);

        layout->addWidget(textEditGroup);

        // Input Validation Section
        QGroupBox *validationGroup = new QGroupBox("Input Validation");
        QVBoxLayout *validationLayout = new QVBoxLayout(validationGroup);

        QHBoxLayout *emailLayout = new QHBoxLayout();
        LabelBase *emailLabel = new LabelBase("Email:", this);
        emailLabel->setMinimumWidth(120);
        LineEditBase *emailEdit = new LineEditBase(this);
        emailEdit->setPlaceholderText("user@example.com");
        emailEdit->setMinimumHeight(35);
        
        PrimaryButton *validateBtn = new PrimaryButton("Validate", this);
        validateBtn->setMinimumSize(100, 35);
        connect(validateBtn, &QPushButton::clicked, this, [this, emailEdit]() {
            QString email = emailEdit->text();
            if (email.contains("@") && email.contains(".")) {
                showMessage("Validation", "Email format is valid!");
            } else {
                showMessage("Validation", "Please enter a valid email address.");
            }
        });

        emailLayout->addWidget(emailLabel);
        emailLayout->addWidget(emailEdit);
        emailLayout->addWidget(validateBtn);
        validationLayout->addLayout(emailLayout);

        layout->addWidget(validationGroup);

        layout->addStretch();
        return widget;
    }

    /**
     * @brief Creates the components demonstration tab
     */
    QWidget* createComponentsTab() {
        QWidget *widget = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(widget);
        layout->setContentsMargins(20, 20, 20, 20);
        layout->setSpacing(20);

        // Labels Section
        QGroupBox *labelsGroup = new QGroupBox("Labels");
        QVBoxLayout *labelsLayout = new QVBoxLayout(labelsGroup);
        labelsLayout->setSpacing(10);

        LabelBase *normalLabel = new LabelBase("This is a normal label", this);
        labelsLayout->addWidget(normalLabel);

        LabelBase *boldLabel = new LabelBase("This is a bold label", this);
        QFont boldFont = boldLabel->font();
        boldFont.setBold(true);
        boldLabel->setFont(boldFont);
        labelsLayout->addWidget(boldLabel);

        LabelBase *largeLabel = new LabelBase("This is a large label", this);
        QFont largeFont = largeLabel->font();
        largeFont.setPointSize(16);
        largeLabel->setFont(largeFont);
        labelsLayout->addWidget(largeLabel);

        layout->addWidget(labelsGroup);

        // CheckBox Section
        QGroupBox *checkBoxGroup = new QGroupBox("CheckBoxes");
        QVBoxLayout *checkBoxLayout = new QVBoxLayout(checkBoxGroup);
        checkBoxLayout->setSpacing(10);

        CheckBoxBase *check1 = new CheckBoxBase("Enable notifications", this);
        checkBoxLayout->addWidget(check1);

        CheckBoxBase *check2 = new CheckBoxBase("Auto-save changes", this);
        check2->setChecked(true);
        checkBoxLayout->addWidget(check2);

        CheckBoxBase *check3 = new CheckBoxBase("Show advanced options", this);
        checkBoxLayout->addWidget(check3);

        layout->addWidget(checkBoxGroup);

        // ComboBox Section
        QGroupBox *comboBoxGroup = new QGroupBox("ComboBox");
        QVBoxLayout *comboBoxLayout = new QVBoxLayout(comboBoxGroup);

        QHBoxLayout *comboLayout = new QHBoxLayout();
        LabelBase *comboLabel = new LabelBase("Select Theme:", this);
        comboLabel->setMinimumWidth(120);
        
        ComboBoxBase *comboBox = new ComboBoxBase(this);
        comboBox->addItem("Light Theme");
        comboBox->addItem("Dark Theme");
        comboBox->addItem("Auto (System)");
        comboBox->setMinimumHeight(35);
        
        comboLayout->addWidget(comboLabel);
        comboLayout->addWidget(comboBox);
        comboLayout->addStretch();
        comboBoxLayout->addLayout(comboLayout);

        layout->addWidget(comboBoxGroup);

        // Interactive Demo Section
        QGroupBox *interactiveGroup = new QGroupBox("Interactive Demo");
        QVBoxLayout *interactiveLayout = new QVBoxLayout(interactiveGroup);

        LabelBase *counterLabel = new LabelBase("Counter: 0", this);
        QFont counterFont = counterLabel->font();
        counterFont.setPointSize(18);
        counterFont.setBold(true);
        counterLabel->setFont(counterFont);
        counterLabel->setAlignment(Qt::AlignCenter);
        interactiveLayout->addWidget(counterLabel);

        QHBoxLayout *counterButtonsLayout = new QHBoxLayout();
        
        static int counter = 0;
        
        PrimaryButton *incrementBtn = new PrimaryButton("Increment", this);
        incrementBtn->setMinimumSize(120, 40);
        connect(incrementBtn, &QPushButton::clicked, this, [counterLabel]() {
            counter++;
            counterLabel->setText(QString("Counter: %1").arg(counter));
        });
        counterButtonsLayout->addWidget(incrementBtn);

        ButtonBase *decrementBtn = new ButtonBase("Decrement", this);
        decrementBtn->setMinimumSize(120, 40);
        connect(decrementBtn, &QPushButton::clicked, this, [counterLabel]() {
            counter--;
            counterLabel->setText(QString("Counter: %1").arg(counter));
        });
        counterButtonsLayout->addWidget(decrementBtn);

        TransparentButton *resetBtn = new TransparentButton("Reset", this);
        resetBtn->setMinimumSize(120, 40);
        connect(resetBtn, &QPushButton::clicked, this, [counterLabel]() {
            counter = 0;
            counterLabel->setText("Counter: 0");
        });
        counterButtonsLayout->addWidget(resetBtn);

        counterButtonsLayout->addStretch();
        interactiveLayout->addLayout(counterButtonsLayout);

        layout->addWidget(interactiveGroup);

        layout->addStretch();
        return widget;
    }

    /**
     * @brief Creates the about tab
     */
    QWidget* createAboutTab() {
        QWidget *widget = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(widget);
        layout->setContentsMargins(40, 40, 40, 40);
        layout->setSpacing(20);

        layout->addStretch();

        // Logo/Title
        LabelBase *logoLabel = new LabelBase("QEasyWidgets", this);
        QFont logoFont = logoLabel->font();
        logoFont.setPointSize(32);
        logoFont.setBold(true);
        logoLabel->setFont(logoFont);
        logoLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(logoLabel);

        // Version
        LabelBase *versionLabel = new LabelBase("Version 1.0.0", this);
        QFont versionFont = versionLabel->font();
        versionFont.setPointSize(14);
        versionLabel->setFont(versionFont);
        versionLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(versionLabel);

        layout->addSpacing(20);

        // Description
        LabelBase *descLabel = new LabelBase(
            "A modern C++ component library for Qt applications\n"
            "featuring beautiful themes and easy-to-use widgets.",
            this
        );
        descLabel->setAlignment(Qt::AlignCenter);
        descLabel->setWordWrap(true);
        layout->addWidget(descLabel);

        layout->addSpacing(30);

        // Features
        QGroupBox *featuresGroup = new QGroupBox("Features");
        QVBoxLayout *featuresLayout = new QVBoxLayout(featuresGroup);
        featuresLayout->setSpacing(10);

        QStringList features = {
            "✓ Modern and beautiful UI components",
            "✓ Light and dark theme support",
            "✓ Frameless window with custom title bar",
            "✓ Enhanced buttons with animations",
            "✓ Improved input widgets",
            "✓ Easy to integrate and customize",
            "✓ Cross-platform compatibility",
            "✓ Qt5 and Qt6 support"
        };

        for (const QString &feature : features) {
            LabelBase *featureLabel = new LabelBase(feature, this);
            QFont featureFont = featureLabel->font();
            featureFont.setPointSize(11);
            featureLabel->setFont(featureFont);
            featuresLayout->addWidget(featureLabel);
        }

        layout->addWidget(featuresGroup);

        layout->addSpacing(20);

        // Action buttons
        QHBoxLayout *actionLayout = new QHBoxLayout();
        actionLayout->addStretch();

        PrimaryButton *githubBtn = new PrimaryButton("View on GitHub", this);
        githubBtn->setMinimumSize(150, 45);
        connect(githubBtn, &QPushButton::clicked, this, [this]() {
            showMessage("GitHub", "This would open the GitHub repository in your browser.");
        });
        actionLayout->addWidget(githubBtn);

        ButtonBase *docsBtn = new ButtonBase("Documentation", this);
        docsBtn->setMinimumSize(150, 45);
        connect(docsBtn, &QPushButton::clicked, this, [this]() {
            showMessage("Documentation", "This would open the documentation.");
        });
        actionLayout->addWidget(docsBtn);

        actionLayout->addStretch();
        layout->addLayout(actionLayout);

        layout->addStretch();

        return widget;
    }

    /**
     * @brief Shows a message dialog
     */
    void showMessage(const QString &title, const QString &message) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(title);
        msgBox.setText(message);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    }
};


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("QEasyWidgets Showcase");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("QEasyWidgets");

    // Create and show main window
    ShowcaseWindow window;
    window.show();

    return app.exec();
}


#include "main.moc"