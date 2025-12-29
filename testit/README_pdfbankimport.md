# PDF Bank Statement Import CLI Tool

A command-line utility for testing PDF bank statement import functionality in Kitupiikki.

## Overview

This tool allows developers to test the PDF bank statement parsing logic without running the full Kitupiikki application. It parses PDF bank statements and outputs the extracted data in JSON format, making it easy to verify import accuracy and debug parsing issues.

## Features

- Parse PDF bank statements from various Finnish banks
- Output parsed data as JSON (compact or pretty-printed)
- Verbose mode for debugging with parsing details
- Calculate and display transaction summaries
- Support for configuring expected IBANs, names, and business IDs

## Building

### Prerequisites

- Qt 5.x or 6.x with PDF module
- C++14 compatible compiler
- qmake build system

### Build Instructions

```bash
cd testit
qmake pdfbankimport.pro
make
```

This will create the `pdfbankimport` executable in the current directory.

## Usage

### Basic Usage

```bash
./pdfbankimport <pdf-file>
```

Example:
```bash
./pdfbankimport data/nordea_statement.pdf
```

### Command-Line Options

| Option | Description |
|--------|-------------|
| `--iban <iban>` | Expected IBAN(s) (comma-separated for multiple accounts) |
| `--name <name>` | Expected account holder name(s) (comma-separated) |
| `--ytunnus <id>` | Business ID (Y-tunnus) for company accounts |
| `--verbose, -v` | Enable verbose output with parsing details (to stderr) |
| `--pretty, -p` | Pretty-print JSON output |
| `--help, -h` | Show help message |
| `--version` | Show version information |

### Examples

#### Simple import with pretty JSON output:
```bash
./pdfbankimport statement.pdf --pretty
```

#### Import with verbose debugging information:
```bash
./pdfbankimport statement.pdf --verbose --pretty
```

#### Import with expected account information:
```bash
./pdfbankimport statement.pdf \
    --iban FI1234567890123456 \
    --name "Example Company Ltd" \
    --ytunnus 1234567-8 \
    --verbose
```

#### Import multiple statements and save to files:
```bash
for file in data/*.pdf; do
    echo "Processing $file..."
    ./pdfbankimport "$file" --pretty > "$(basename "$file" .pdf).json"
done
```

#### Redirect only JSON output (without verbose info):
```bash
./pdfbankimport statement.pdf --pretty > output.json
```

## Output Format

The tool outputs JSON to stdout with the following structure:

```json
{
  "tyyppi": 400,
  "iban": "FI1234567890123456",
  "kausitunnus": "1/2024",
  "alkupvm": "2024-01-01",
  "loppupvm": "2024-01-31",
  "tapahtumat": [
    {
      "pvm": "2024-01-15",
      "euro": "123.45",
      "saajamaksaja": "Company Name",
      "viite": "12345",
      "selite": "Payment description",
      "arkistotunnus": "2024011512345",
      "ktokoodi": 700
    }
  ]
}
```

### Field Descriptions

| Field | Description |
|-------|-------------|
| `tyyppi` | Document type (400 = bank statement) |
| `iban` | Account IBAN |
| `kausitunnus` | Period identifier (e.g., "1/2024") |
| `alkupvm` | Statement start date (ISO format) |
| `loppupvm` | Statement end date (ISO format) |
| `tapahtumat` | Array of transactions |

### Transaction Fields

| Field | Description |
|-------|-------------|
| `pvm` | Transaction date (ISO format) |
| `euro` | Amount as string (negative for withdrawals) |
| `saajamaksaja` | Payee or payer name |
| `viite` | Bank reference number |
| `selite` | Transaction description |
| `arkistotunnus` | Archive/transaction ID |
| `ktokoodi` | KTO code (account type) |

## Verbose Output

When using `--verbose`, the tool prints additional information to stderr:

```
Loading PDF: statement.pdf
PDF loaded successfully
Pages: 3
Parsing PDF...
Parsing completed

=== Import Summary ===
Document Type: 400
IBAN: FI1234567890123456
Period: 1/2024
Date Range: 2024-01-01 to 2024-01-31
Transactions: 45
Total Deposits: 12,345.67 €
Total Withdrawals: 8,901.23 €
Net: 3,444.44 €
======================
```

## Testing Workflow

### 1. Test a single PDF file:
```bash
./pdfbankimport data/test_statement.pdf --verbose --pretty
```

### 2. Compare against expected results:
```bash
./pdfbankimport data/test_statement.pdf > actual.json
diff expected.json actual.json
```

### 3. Batch testing with validation:
```bash
for pdf in data/test_*.pdf; do
    echo "Testing $pdf"
    ./pdfbankimport "$pdf" --verbose 2>&1 | grep -E "(ERROR|Transactions:)"
done
```

### 4. Extract specific information with jq:
```bash
# Get transaction count
./pdfbankimport statement.pdf | jq '.tapahtumat | length'

# Get total amount
./pdfbankimport statement.pdf | jq '[.tapahtumat[].euro | tonumber] | add'

# List all payees
./pdfbankimport statement.pdf | jq -r '.tapahtumat[].saajamaksaja'
```

## Error Handling

The tool returns the following exit codes:

- `0` - Success
- `1` - Error (file not found, PDF loading failed, parsing error)

Error messages are printed to stderr:

```bash
ERROR: File not found: nonexistent.pdf
ERROR: Failed to load PDF: corrupted.pdf
Reason: Unable to load document
```

## Integration with Existing Tests

This tool complements the existing test infrastructure:

- **pdftesti** - Runs batch tests with JSON specifications
- **tiliotetesti** - Legacy bank statement testing
- **pdfbankimport** (this tool) - Interactive single-file testing

Use this tool for:
- Manual testing of new PDF formats
- Debugging parsing issues
- Quick validation of import logic changes
- Creating test fixtures for automated tests

## Development

The tool uses the following components from the Kitupiikki codebase:

- `Tuonti::PdfTiedosto` - PDF file handler
- `Tuonti::PdfTilioteTuonti` - Bank statement parser
- `Tuonti::TuontiApuInfo` - Import context information
- `Euro` - Currency amount handling

See the main Kitupiikki documentation for details on these components.

## Troubleshooting

### Build Issues

**Problem:** `fatal error: QPdfDocument: No such file or directory`
**Solution:** Make sure Qt PDF module is installed:
```bash
# On Ubuntu/Debian
sudo apt-get install libqt5pdf5-dev

# On Fedora
sudo dnf install qt5-qtwebengine-devel
```

**Problem:** Linking errors with PDF import symbols
**Solution:** Verify that `pdftuonti.pri` is included correctly in the .pro file

### Runtime Issues

**Problem:** Segmentation fault when loading PDF
**Solution:** Check that the PDF file is valid and not corrupted. Try opening it with another PDF viewer first.

**Problem:** No transactions extracted from valid PDF
**Solution:** Enable verbose mode to see parsing details. The PDF format may not be recognized by the parser.

## Contributing

When adding support for new bank PDF formats:

1. Test with this CLI tool first
2. Add test case to `pdftesti` JSON specifications
3. Update parsing logic in `PdfTilioteTuonti`
4. Verify with both tools

## License

This tool is part of the Kitupiikki project. See the main project LICENSE file.
