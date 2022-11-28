#include "windows/optionsdialog.h"
#include "windows/devicemanager.h"
#include "windows/keymanager.h"
#include "widgets/filebox.h"
#include "tools/adb.h"
#include "tools/apksigner.h"
#include "tools/apktool.h"
#include "tools/zipalign.h"
#include "base/application.h"
#include "base/settings.h"
#include "base/themes.h"
#include "base/utils.h"
#include <QAbstractButton>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

OptionsDialog::OptionsDialog(QWidget *parent) : QDialog(parent)
{
    widget = nullptr;
    layout = new QVBoxLayout(this);
    initialize();
}

void OptionsDialog::addPage(const QString &title, QLayout *page, bool stretch)
{
    auto container = new QWidget(this);
    auto containerLayout = new QVBoxLayout(container);
    auto titleLabel = new QLabel(QString("%1").arg(title), this);
    auto titleLine = new QFrame(this);
    titleLine->setFrameShape(QFrame::HLine);
    titleLine->setFrameShadow(QFrame::Sunken);
    QFont titleFont = titleLabel->font();
#ifndef Q_OS_MACOS
    titleFont.setPointSize(13);
#else
    titleFont.setPointSize(17);
#endif
    titleLabel->setFont(titleFont);
    containerLayout->addWidget(titleLabel);
    containerLayout->addWidget(titleLine);
    containerLayout->addLayout(page);
    if (stretch) {
        containerLayout->addStretch();
    }
    pageList->addItem(title);
    pageStack->addWidget(container);
}

void OptionsDialog::load()
{
    // General

    checkboxSingleInstance->setChecked(app->settings->getSingleInstance());
    checkboxUpdates->setChecked(app->settings->getAutoUpdates());
    spinboxRecent->setValue(app->settings->getRecentLimit());
#ifdef Q_OS_WIN
    groupAssociate->setChecked(app->settings->getFileAssociation());
    checkboxExplorerOpen->setChecked(app->settings->getExplorerOpenIntegration());
    checkboxExplorerInstall->setChecked(app->settings->getExplorerInstallIntegration());
    checkboxExplorerOptimize->setChecked(app->settings->getExplorerOptimizeIntegration());
    checkboxExplorerSign->setChecked(app->settings->getExplorerSignIntegration());
#endif

    // Interface

    checkboxDarkMode->setChecked(app->settings->getDarkMode());
    checkboxTray->setChecked(app->settings->getTrayIcon());
    checkboxTrayClose->setChecked(app->settings->getTrayClose());
    checkboxTrayMinimize->setChecked(app->settings->getTrayMinimize());
    checkboxTrayStart->setChecked(app->settings->getTrayStart());
    groupTray->setChecked(app->settings->getTrayEnabled());
    checkboxTaskbar->setChecked(app->settings->getTaskbarProgress());
    checkboxTaskbarStart->setChecked(app->settings->getTaskbarStart());
    checkboxTaskbarFinish->setChecked(app->settings->getTaskbarFinish());
    checkboxTaskbarErrors->setChecked(app->settings->getTaskbarErrors());
    checkboxTaskbarWarnings->setChecked(app->settings->getTaskbarWarnings());
    groupTaskbar->setChecked(app->settings->getTaskbarEnabled());
    checkboxNotifications->setChecked(app->settings->getNotifications());
    checkboxNotificationsStart->setChecked(app->settings->getNotificationsStart());
    checkboxNotificationsFinish->setChecked(app->settings->getNotificationsFinish());
    checkboxNotificationsErrors->setChecked(app->settings->getNotificationsErrors());
    checkboxNotificationsWarnings->setChecked(app->settings->getNotificationsWarnings());
    groupNotifications->setChecked(app->settings->getNotificationsEnabled());

    // Tools

    editAdb->setText(app->settings->getAdbPath());
    editApktool->setText(app->settings->getApktoolPath());
    editApksigner->setText(app->settings->getApksignerPath());
    editZipalign->setText(app->settings->getZipalignPath());

    // Devices

    deviceManager->load();
}

    // Appearance

    comboLanguages->clear();
    const QList<Language> languages = app->getLanguages();
    const QString currentLocale = app->settings->getLanguage();
    for (const Language &language : languages) {
        const QIcon flag = language.getFlag();
        const QString title = language.getTitle();
        const QString code = language.getCode();
        comboLanguages->addItem(flag, title, code);
        if (code == currentLocale) {
            comboLanguages->setCurrentIndex(comboLanguages->count() - 1);
        }
    }
    comboLanguages->setCurrentText(app->settings->getLanguage());

    const int themeIndex = comboThemes->findData(app->settings->getTheme());
    comboThemes->setCurrentIndex(themeIndex != -1 ? themeIndex : 0);

    // Java

    fileboxJava->setCurrentPath(app->settings->getJavaPath());
    spinboxMinHeapSize->setValue(app->settings->getJavaMinHeapSize());
    spinboxMaxHeapSize->setValue(app->settings->getJavaMaxHeapSize());

    // Apktool

    fileboxApktool->setCurrentPath(app->settings->getApktoolPath());
    fileboxOutput->setCurrentPath(app->settings->getOutputDirectory());
    fileboxFrameworks->setCurrentPath(app->settings->getFrameworksDirectory());
    checkboxAapt2->setChecked(app->settings->getUseAapt2());
    checkboxDebuggable->setChecked(app->settings->getMakeDebuggable());
    checkboxSources->setChecked(app->settings->getDecompileSources());
    checkboxBrokenResources->setChecked(app->settings->getKeepBrokenResources());

    // Apksigner

    checkboxSign->setChecked(app->settings->getSignApk());
    fileboxApksigner->setCurrentPath(app->settings->getApksignerPath());

    // Zipalign

    checkboxZipalign->setChecked(app->settings->getOptimizeApk());
    fileboxZipalign->setCurrentPath(app->settings->getZipalignPath());

    // ADB

    fileboxAdb->setCurrentPath(app->settings->getAdbPath());
}

void OptionsDialog::save()
{
    // General

    app->settings->setSingleInstance(checkboxSingleInstance->isChecked());
    app->settings->setAutoUpdates(checkboxUpdates->isChecked());
    app->settings->setRecentLimit(spinboxRecent->value());
#ifdef Q_OS_WIN
    bool integrationSuccess =
        app->settings->setFileAssociation(groupAssociate->isChecked()) &&
        app->settings->setExplorerOpenIntegration(checkboxExplorerOpen->isChecked()) &&
        app->settings->setExplorerInstallIntegration(checkboxExplorerInstall->isChecked()) &&
        app->settings->setExplorerOptimizeIntegration(checkboxExplorerOptimize->isChecked()) &&
        app->settings->setExplorerSignIntegration(checkboxExplorerSign->isChecked());
    if (!integrationSuccess) {
        QMessageBox::warning(this, QString(), tr("Could not register file association."));
    }
#endif

    // Appearance

    app->setLanguage(comboLanguages->currentData().toString());
    app->settings->setTheme(comboThemes->currentData().toString());

    // Java

    app->settings->setJavaPath(fileboxJava->getCurrentPath());
    app->settings->setJavaMinHeapSize(spinboxMinHeapSize->value());
    app->settings->setJavaMaxHeapSize(spinboxMaxHeapSize->value());

    // Apktool

    app->settings->setApktoolPath(fileboxApktool->getCurrentPath());
    app->settings->setOutputDirectory(fileboxOutput->getCurrentPath());
    app->settings->setFrameworksDirectory(fileboxFrameworks->getCurrentPath());
    app->settings->setUseAapt2(checkboxAapt2->isChecked());
    app->settings->setMakeDebuggable(checkboxDebuggable->isChecked());
    app->settings->setDecompileSources(checkboxSources->isChecked());
    app->settings->setKeepBrokenResources(checkboxBrokenResources->isChecked());

    // Apksigner

    app->settings->setSignApk(checkboxSign->isChecked());
    app->settings->setApksignerPath(fileboxApksigner->getCurrentPath());

    // Zipalign

    app->settings->setOptimizeApk(checkboxZipalign->isChecked());
    app->settings->setZipalignPath(fileboxZipalign->getCurrentPath());

    // ADB

    app->settings->setAdbPath(fileboxAdb->getCurrentPath());
}

void OptionsDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
#ifdef QT_DEBUG
        qDebug() << "TODO Unwanted behavior: this event is fired twice";
#endif
        const int currentPage = pageList->currentRow();
        initialize();
        pageList->setCurrentRow(currentPage);
    }
    QDialog::changeEvent(event);
}

void OptionsDialog::initialize()
{
    // Clear layout:

    if (widget) {
        delete widget;
    }

    widget = new QWidget(this);
    layout->addWidget(widget);

    setWindowTitle(tr("Options"));
    setWindowIcon(QIcon::fromTheme("configure"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(Utils::scale(800, 400));

    // General

    auto pageGeneral = new QFormLayout;
    checkboxSingleInstance = new QCheckBox(tr("Single-window mode"), this);
    checkboxUpdates = new QCheckBox(tr("Check for updates automatically"), this);
    spinboxRecent = new QSpinBox(this);
    spinboxRecent->setMinimum(0);
    spinboxRecent->setMaximum(50);
#ifdef Q_OS_MACOS
    checkboxSingleInstance->hide();
#endif
    pageGeneral->addRow(checkboxSingleInstance);
    pageGeneral->addRow(checkboxUpdates);
    pageGeneral->addRow(tr("Maximum recent files:"), spinboxRecent);

#ifdef Q_OS_WIN
    //: Don't translate the "APK Editor Studio" and ".apk" parts.
    groupAssociate = new QGroupBox(tr("Use APK Editor Studio for .apk files"), this);
    groupAssociate->setCheckable(true);
    //: Don't translate the "APK Editor Studio" and ".apk" parts.
    checkboxExplorerOpen = new QCheckBox(tr("Use APK Editor Studio to open .apk files"), this);
    checkboxExplorerOpen->setIcon(QIcon::fromTheme("document-open"));
    //: "%1" will be replaced with an action name (e.g., Install, Optimize, Sign, etc.).
    const QString strExplorerIntegration(tr("Add %1 action to Windows Explorer context menu"));
    checkboxExplorerInstall = new QCheckBox(strExplorerIntegration.arg(tr("Install")), this);
    checkboxExplorerInstall->setIcon(QIcon::fromTheme("apk-install"));
    checkboxExplorerOptimize = new QCheckBox(strExplorerIntegration.arg(tr("Optimize")), this);
    checkboxExplorerOptimize->setIcon(QIcon::fromTheme("apk-optimize"));
    //: This is a verb.
    checkboxExplorerSign = new QCheckBox(strExplorerIntegration.arg(tr("Sign")), this);
    checkboxExplorerSign->setIcon(QIcon::fromTheme("apk-sign"));
    connect(groupAssociate, &QGroupBox::clicked, this, [this](bool checked) {
        checkboxExplorerOpen->setChecked(checked);
        checkboxExplorerInstall->setChecked(checked);
        checkboxExplorerOptimize->setChecked(checked);
        checkboxExplorerSign->setChecked(checked);
    });
    auto layoutAssocate = new QVBoxLayout(groupAssociate);
    layoutAssocate->addWidget(checkboxExplorerOpen);
    layoutAssocate->addWidget(checkboxExplorerInstall);
    layoutAssocate->addWidget(checkboxExplorerOptimize);
    layoutAssocate->addWidget(checkboxExplorerSign);
    pageGeneral->addRow(groupAssociate);
#endif

    // Appearance

    comboLanguages = new QComboBox(this);

    auto pageAppearance = new QFormLayout;
    comboThemes = new QComboBox(this);
    ThemeRepository themes;
    const auto names = themes.getThemeNames();
    for (const QString &name : names) {
        comboThemes->addItem(themes.getTheme(name)->title(), name);
    }

    pageAppearance->addRow(tr("Language:"), comboLanguages);
    pageAppearance->addRow(tr("Theme:"), comboThemes);

    // Java

    auto pageJava = new QFormLayout;
    fileboxJava = new FileBox(true, this);
    fileboxJava->setDefaultPath("");
    fileboxJava->setPlaceholderText(tr("Extracted from environment variables by default"));
    spinboxMinHeapSize = new QSpinBox(this);
    spinboxMaxHeapSize = new QSpinBox(this);
    //: Megabytes
    const QString heapSizeSuffix = QString(" %1").arg(tr("MB"));
    spinboxMinHeapSize->setSuffix(heapSizeSuffix);
    spinboxMaxHeapSize->setSuffix(heapSizeSuffix);
    spinboxMinHeapSize->setSpecialValueText(tr("Default"));
    spinboxMaxHeapSize->setSpecialValueText(tr("Default"));
    spinboxMinHeapSize->setRange(0, std::numeric_limits<int>::max());
    spinboxMaxHeapSize->setRange(0, std::numeric_limits<int>::max());
    pageJava->addRow(tr("Java path:"), fileboxJava);
    //: "Heap" refers to a memory heap. If there is no clear translation in your language, you may also put the original English word in the parentheses.
    pageJava->addRow(tr("Initial heap size:"), spinboxMinHeapSize);
    //: "Heap" refers to a memory heap. If there is no clear translation in your language, you may also put the original English word in the parentheses.
    pageJava->addRow(tr("Maximum heap size:"), spinboxMaxHeapSize);

    // Apktool

    auto pageApktool = new QGridLayout;

    fileboxApktool = new FileBox(false, this);
    fileboxApktool->setDefaultPath("");
    fileboxApktool->setPlaceholderText(Apktool::getDefaultPath());
    fileboxOutput = new FileBox(true, this);
    fileboxOutput->setDefaultPath("");
    fileboxOutput->setPlaceholderText(Apktool::getDefaultOutputPath());
    fileboxFrameworks = new FileBox(true, this);
    fileboxFrameworks->setDefaultPath("");
    fileboxFrameworks->setPlaceholderText(Apktool::getDefaultFrameworksPath());
    auto formApktool = new QFormLayout;
    //: "Apktool" is the name of the tool, don't translate it.
    formApktool->addRow(tr("Apktool path:"), fileboxApktool);
    formApktool->addRow(tr("Extraction path:"), fileboxOutput);
    formApktool->addRow(tr("Frameworks path:"), fileboxFrameworks);
    formApktool->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    auto groupUnpacking = new QGroupBox(tr("Unpacking"), this);
    //: "Smali" is the name of the tool/format, don't translate it.
    checkboxSources = new QCheckBox(tr("Decompile source code (smali)"), this);
    checkboxBrokenResources = new QCheckBox(tr("Decompile broken resources"), this);
    auto layoutUnpacking = new QVBoxLayout(groupUnpacking);
    layoutUnpacking->addWidget(checkboxSources);
    layoutUnpacking->addWidget(checkboxBrokenResources);

    auto groupPacking = new QGroupBox(tr("Packing"), this);
    //: "AAPT2" is the name of the tool, don't translate it.
    checkboxAapt2 = new QCheckBox(tr("Use AAPT2"), this);
    checkboxDebuggable = new QCheckBox(tr("Pack for debugging"), this);
    auto layoutPacking = new QVBoxLayout(groupPacking);
    layoutPacking->addWidget(checkboxAapt2);
    layoutPacking->addWidget(checkboxDebuggable);

    pageApktool->addLayout(formApktool, 0, 0, 1, 2);
    pageApktool->addWidget(groupUnpacking, 1, 0);
    pageApktool->addWidget(groupPacking, 1, 1);

    // Signing

    auto pageSigning = new QFormLayout;
    fileboxKeyStore = new FileBox(true, this);
    fileboxKeyStore->setDefaultPath("");
    fileboxKeyStore->setPlaceholderText(Signer::getDefaultKeyStorePath());
    lineEditAlias = new QLineEdit(this);
    lineEditAlias->setPlaceholderText(Signer::getDefaultAlias());
    lineEditAlias->setClearButtonEnabled(true);
    lineEditPassword = new QLineEdit(this);
    lineEditPassword->setPlaceholderText(Signer::getDefaultKeyStorePassword());
    lineEditPassword->setEchoMode(QLineEdit::Password);
    lineEditPassword->setClearButtonEnabled(true);
    lineEditKeyPassword = new QLineEdit(this);
    lineEditKeyPassword->setPlaceholderText(Signer::getDefaultKeyPassword());
    lineEditKeyPassword->setEchoMode(QLineEdit::Password);
    lineEditKeyPassword->setClearButtonEnabled(true);
    pageSigning->addRow(tr("Key store path:"), fileboxKeyStore);
    pageSigning->addRow(tr("Alias:"), lineEditAlias);
    pageSigning->addRow(tr("Key store password:"), lineEditPassword);
    pageSigning->addRow(tr("Key password:"), lineEditKeyPassword);

    // Main

    auto layout = new QVBoxLayout(this);
    auto tabs = new QTabWidget(this);
    tabs->addTab(pageGeneral, tr("General"));
    tabs->addTab(pageAppearance, tr("Appearance"));
    tabs->addTab(pageJava, tr("Java"));
    tabs->addTab(pageApktool, tr("Apktool"));
    tabs->addTab(pageSigning, tr("Signing"));
    layout->addWidget(tabs);

    auto buttons = new QDialogButtonBox(this);
    buttons->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);
    layout->addWidget(buttons);

    installEventFilter(this);

    retranslate();
    loadSettings();
}

    // Apksigner

    auto pageApksigner = new QFormLayout;
    checkboxSign = new QCheckBox(tr("Sign APK after packing"), this);
    fileboxApksigner = new FileBox(false, this);
    fileboxApksigner->setDefaultPath("");
    fileboxApksigner->setPlaceholderText(Apksigner::getDefaultPath());
    //: This string refers to multiple keys (as in "Manager of keys").
    auto btnKeyManager = new QPushButton(tr("Open Key Manager"), this);
    btnKeyManager->setIcon(QIcon::fromTheme("apk-sign"));
    btnKeyManager->setMinimumHeight(Utils::scale(30));
    connect(btnKeyManager, &QPushButton::clicked, this, [this]() {
        KeyManager keyManager(this);
        keyManager.exec();
    });
    pageApksigner->addRow(checkboxSign);
    //: "Apksigner" is the name of the tool, don't translate it.
    pageApksigner->addRow(tr("Apksigner path:"), fileboxApksigner);
    pageApksigner->addRow(btnKeyManager);
    pageApksigner->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);


    // Zipalign

    auto pageZipalign = new QFormLayout;
    checkboxZipalign = new QCheckBox(tr("Optimize APK after packing"), this);
    fileboxZipalign = new FileBox(false, this);
    fileboxZipalign->setDefaultPath("");
    fileboxZipalign->setPlaceholderText(Zipalign::getDefaultPath());
    pageZipalign->addRow(checkboxZipalign);
    //: "Zipalign" is the name of the tool, don't translate it.
    pageZipalign->addRow(tr("Zipalign path:"), fileboxZipalign);
    pageZipalign->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    // ADB

    auto pageAdb = new QFormLayout;
    fileboxAdb = new FileBox(false, this);
    fileboxAdb->setDefaultPath("");
    fileboxAdb->setPlaceholderText(Adb::getDefaultPath());
    //: This string refers to multiple devices (as in "Manager of devices").
    auto btnDeviceManager = new QPushButton(tr("Open Device Manager"), this);
    btnDeviceManager->setIcon(QIcon::fromTheme("smartphone"));
    btnDeviceManager->setMinimumHeight(Utils::scale(30));
    connect(btnDeviceManager, &QPushButton::clicked, this, [this]() {
        DeviceManager deviceManager(this);
        deviceManager.exec();
    });
    //: "ADB" is the name of the tool, don't translate it.
    pageAdb->addRow(tr("ADB path:"), fileboxAdb);
    pageAdb->addRow(btnDeviceManager);
    pageAdb->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    // Main

    auto layout = new QVBoxLayout(this);
    auto tabs = new QTabWidget(this);
    tabs->addTab(pageGeneral, tr("General"));
    tabs->addTab(pageAppearance, tr("Appearance"));
    tabs->addTab(pageJava, tr("Java"));
    tabs->addTab(pageApktool, tr("Apktool"));
    tabs->addTab(pageApksigner, tr("Apksigner"));
    tabs->addTab(pageZipalign, tr("Zipalign"));
    tabs->addTab(pageAdb, tr("ADB"));
    layout->addWidget(tabs);

    auto buttons = new QDialogButtonBox(this);
    buttons->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);
    layout->addWidget(buttons);

    installEventFilter(this);

    retranslate();
    loadSettings();
}

    // Initialize

    pageStack = new QStackedWidget(this);
    pageStack->setFrameShape(QFrame::StyledPanel);
    pageList = new QListWidget(this);
    addPage(tr("General"), pageGeneral);
    addPage(tr("Appearance"), pageAppearance);
    addPage("Java", pageJava);
    addPage("Apktool", pageApktool);
    addPage("Apksigner", pageApksigner);
    addPage("Zipalign", pageZipalign);
    addPage("ADB", pageAdb);
    pageList->setCurrentRow(0);
    pageList->setMaximumWidth(pageList->sizeHintForColumn(0) + 60);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply, this);
    QPushButton *btnApply = buttons->button(QDialogButtonBox::Apply);

    QGridLayout *layoutPages = new QGridLayout(widget);
    layoutPages->addWidget(pageList, 0, 0);
    layoutPages->addWidget(pageStack, 0, 1);
    layoutPages->addWidget(buttons, 1, 0, 1, 2);

    load();

    connect(pageList, &QListWidget::currentRowChanged, pageStack, &QStackedWidget::setCurrentIndex);
    connect(comboThemes, &QComboBox::currentTextChanged, this, [=]() {
        QMessageBox::information(this, {}, tr("The changes will take effect after the application restart."));
    });
    connect(buttons, &QDialogButtonBox::accepted, this, &OptionsDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &OptionsDialog::reject);
    connect(btnApply, &QPushButton::clicked, this, &OptionsDialog::save);
    connect(this, &OptionsDialog::accepted, this, &OptionsDialog::save);
}
