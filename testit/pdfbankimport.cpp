/*
 * PDF Bank Statement Import CLI Tool
 *
 * A command-line utility for testing PDF bank statement import functionality.
 * This tool parses PDF bank statements and outputs the extracted data in JSON format.
 *
 * Usage:
 *   pdfbankimport <pdf-file> [options]
 *
 * Options:
 *   --iban <iban>        Expected IBAN(s) (comma-separated for multiple)
 *   --name <name>        Expected account holder name(s) (comma-separated for multiple)
 *   --ytunnus <id>       Business ID (Y-tunnus)
 *   --verbose, -v        Verbose output with parsing details
 *   --pretty, -p         Pretty-print JSON output
 *   --help, -h           Show this help message
 */

#include <QCoreApplication>
#include <QFile>
#include <QCommandLineParser>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantMap>
#include <QPdfDocument>
#include <iostream>
#include <QTimer>

#include "../kitsas/tuonti/pdf/pdftiedosto.h"
#include "../kitsas/tuonti/pdf/tuontiapuinfo.h"
#include "../kitsas/model/euro.h"

class ImportApuInfo : public Tuonti::TuontiApuInfo {
public:
    ImportApuInfo(const QStringList& ibans, const QStringList& names, const QString& ytunnus) {
        omatIbanit_ = ibans;
        omatNimet_ = names;
        yTunnus_ = ytunnus;
    }
};

class PdfImporter : public QObject {
    Q_OBJECT

public:
    PdfImporter(const QString& pdfPath,
                const QStringList& ibans,
                const QStringList& names,
                const QString& ytunnus,
                bool verbose,
                bool pretty,
                QCoreApplication* app)
        : pdfPath_(pdfPath)
        , ibans_(ibans)
        , names_(names)
        , ytunnus_(ytunnus)
        , verbose_(verbose)
        , pretty_(pretty)
        , app_(app)
        , pdfDoc_(new QPdfDocument(this))
    {
        connect(pdfDoc_, &QPdfDocument::statusChanged, this, &PdfImporter::onPdfLoaded);
    }

    void import() {
        if (verbose_) {
            std::cerr << "Loading PDF: " << pdfPath_.toStdString() << std::endl;
        }
        pdfDoc_->load(pdfPath_);
    }

private slots:
    void onPdfLoaded(QPdfDocument::Status status) {
        if (status == QPdfDocument::Status::Ready) {
            if (verbose_) {
                std::cerr << "PDF loaded successfully" << std::endl;
                std::cerr << "Pages: " << pdfDoc_->pageCount() << std::endl;
            }

            ImportApuInfo info(ibans_, names_, ytunnus_);
            Tuonti::PdfTiedosto pdfTiedosto(pdfDoc_);

            if (verbose_) {
                std::cerr << "Parsing PDF..." << std::endl;
            }

            QVariantMap result = pdfTiedosto.tuo(info);

            if (verbose_) {
                std::cerr << "Parsing completed" << std::endl;
                printSummary(result);
            }

            printJson(result);
            app_->exit(0);

        } else if (status == QPdfDocument::Status::Error) {
            std::cerr << "ERROR: Failed to load PDF: " << pdfPath_.toStdString() << std::endl;
            std::cerr << "Reason: " << pdfDoc_->error().toStdString() << std::endl;
            app_->exit(1);

        } else if (status == QPdfDocument::Status::Loading) {
            if (verbose_) {
                std::cerr << "Loading PDF..." << std::endl;
            }
        }
    }

private:
    void printSummary(const QVariantMap& result) {
        std::cerr << "\n=== Import Summary ===" << std::endl;
        std::cerr << "Document Type: " << result.value("tyyppi").toInt() << std::endl;
        std::cerr << "IBAN: " << result.value("iban").toString().toStdString() << std::endl;
        std::cerr << "Period: " << result.value("kausitunnus").toString().toStdString() << std::endl;

        QDate startDate = result.value("alkupvm").toDate();
        QDate endDate = result.value("loppupvm").toDate();

        if (startDate.isValid() && endDate.isValid()) {
            std::cerr << "Date Range: " << startDate.toString(Qt::ISODate).toStdString()
                      << " to " << endDate.toString(Qt::ISODate).toStdString() << std::endl;
        }

        QVariantList transactions = result.value("tapahtumat").toList();
        std::cerr << "Transactions: " << transactions.size() << std::endl;

        // Calculate totals
        Euro deposits;
        Euro withdrawals;

        for (const auto& item : transactions) {
            QVariantMap tmap = item.toMap();
            Euro amount(tmap.value("euro").toString());
            if (amount < Euro::Zero) {
                withdrawals += amount.abs();
            } else {
                deposits += amount;
            }
        }

        std::cerr << "Total Deposits: " << deposits.display().toStdString() << std::endl;
        std::cerr << "Total Withdrawals: " << withdrawals.display().toStdString() << std::endl;
        std::cerr << "Net: " << (deposits - withdrawals).display().toStdString() << std::endl;
        std::cerr << "======================\n" << std::endl;
    }

    void printJson(const QVariantMap& result) {
        QJsonDocument doc = QJsonDocument::fromVariant(result);

        if (pretty_) {
            std::cout << doc.toJson(QJsonDocument::Indented).toStdString();
        } else {
            std::cout << doc.toJson(QJsonDocument::Compact).toStdString();
        }
        std::cout << std::endl;
    }

    QString pdfPath_;
    QStringList ibans_;
    QStringList names_;
    QString ytunnus_;
    bool verbose_;
    bool pretty_;
    QCoreApplication* app_;
    QPdfDocument* pdfDoc_;
};

void printHelp(const QString& programName) {
    std::cout << "PDF Bank Statement Import CLI Tool\n\n";
    std::cout << "Usage: " << programName.toStdString() << " <pdf-file> [options]\n\n";
    std::cout << "Arguments:\n";
    std::cout << "  pdf-file              Path to the PDF bank statement file\n\n";
    std::cout << "Options:\n";
    std::cout << "  --iban <iban>         Expected IBAN(s) (comma-separated for multiple)\n";
    std::cout << "  --name <name>         Expected account holder name(s) (comma-separated)\n";
    std::cout << "  --ytunnus <id>        Business ID (Y-tunnus)\n";
    std::cout << "  --verbose, -v         Verbose output with parsing details\n";
    std::cout << "  --pretty, -p          Pretty-print JSON output\n";
    std::cout << "  --help, -h            Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName.toStdString() << " statement.pdf\n";
    std::cout << "  " << programName.toStdString() << " statement.pdf --verbose --pretty\n";
    std::cout << "  " << programName.toStdString() << " statement.pdf --iban FI1234567890123456 --name \"Company Ltd\"\n\n";
    std::cout << "Output:\n";
    std::cout << "  The tool outputs parsed data as JSON to stdout.\n";
    std::cout << "  Use --verbose to see parsing details on stderr.\n";
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("pdfbankimport");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("PDF Bank Statement Import CLI Tool for testing import logic");

    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("file", "PDF bank statement file to import");

    QCommandLineOption ibanOption(QStringList() << "iban",
        "Expected IBAN(s) (comma-separated for multiple)",
        "iban");
    parser.addOption(ibanOption);

    QCommandLineOption nameOption(QStringList() << "name",
        "Expected account holder name(s) (comma-separated for multiple)",
        "name");
    parser.addOption(nameOption);

    QCommandLineOption ytunnusOption(QStringList() << "ytunnus",
        "Business ID (Y-tunnus)",
        "ytunnus");
    parser.addOption(ytunnusOption);

    QCommandLineOption verboseOption(QStringList() << "v" << "verbose",
        "Verbose output with parsing details");
    parser.addOption(verboseOption);

    QCommandLineOption prettyOption(QStringList() << "p" << "pretty",
        "Pretty-print JSON output");
    parser.addOption(prettyOption);

    parser.process(app);

    QStringList positionalArgs = parser.positionalArguments();

    if (positionalArgs.isEmpty()) {
        std::cerr << "ERROR: No PDF file specified\n\n";
        printHelp(QCoreApplication::applicationName());
        return 1;
    }

    QString pdfPath = positionalArgs.first();

    // Check if file exists
    QFile file(pdfPath);
    if (!file.exists()) {
        std::cerr << "ERROR: File not found: " << pdfPath.toStdString() << std::endl;
        return 1;
    }

    // Parse options
    QStringList ibans;
    if (parser.isSet(ibanOption)) {
        QString ibanString = parser.value(ibanOption);
        ibans = ibanString.split(',', Qt::SkipEmptyParts);
        for (QString& iban : ibans) {
            iban = iban.trimmed();
        }
    }

    QStringList names;
    if (parser.isSet(nameOption)) {
        QString nameString = parser.value(nameOption);
        names = nameString.split(',', Qt::SkipEmptyParts);
        for (QString& name : names) {
            name = name.trimmed();
        }
    }

    QString ytunnus = parser.value(ytunnusOption);
    bool verbose = parser.isSet(verboseOption);
    bool pretty = parser.isSet(prettyOption);

    // Create importer and start import
    PdfImporter importer(pdfPath, ibans, names, ytunnus, verbose, pretty, &app);

    // Use QTimer to start import after event loop begins
    QTimer::singleShot(0, &importer, &PdfImporter::import);

    return app.exec();
}

#include "pdfbankimport.moc"
