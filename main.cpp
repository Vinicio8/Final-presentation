#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
using namespace std;

class NewtonInterpolator {
private:
    vector<double> x_values;
    vector<double> y_values;
    vector<vector<double>> divided_differences;
    vector<double> current_polynomial;
    bool needs_update;

    void computeDividedDifferences() {
        int n = x_values.size();
        divided_differences.resize(n, vector<double>(n, 0.0));

        for (int i = 0; i < n; i++) {
            divided_differences[i][0] = y_values[i];
        }

        for (int j = 1; j < n; j++) {
            for (int i = 0; i < n - j; i++) {
                divided_differences[i][j] = 
                    (divided_differences[i+1][j-1] - divided_differences[i][j-1]) / 
                    (x_values[i+j] - x_values[i]);
            }
        }
    }

    vector<double> multiplyPolynomials(const vector<double>& p1, const vector<double>& p2) {
        vector<double> result(p1.size() + p2.size() - 1, 0.0);
        for (int i = 0; i < p1.size(); i++) {
            for (int j = 0; j < p2.size(); j++) {
                result[i + j] += p1[i] * p2[j];
            }
        }
        return result;
    }

    vector<double> addPolynomials(const vector<double>& p1, const vector<double>& p2) {
        int max_size = max(p1.size(), p2.size());
        vector<double> result(max_size, 0.0);
        for (int i = 0; i < p1.size(); i++) result[i] += p1[i];
        for (int i = 0; i < p2.size(); i++) result[i] += p2[i];
        return result;
    }

public:
    NewtonInterpolator() : needs_update(false) {}

    void addPoint(double x, double y) {
        // Check if x already exists
        for (size_t i = 0; i < x_values.size(); i++) {
            if (abs(x_values[i] - x) < 1e-10) {
                cout << "Warning: x value " << x << " already exists. Updating y value." << endl;
                y_values[i] = y;
                needs_update = true;
                return;
            }
        }

        // Insert the new point in sorted order for better numerical stability
        auto it_x = lower_bound(x_values.begin(), x_values.end(), x);
        size_t pos = distance(x_values.begin(), it_x);

        x_values.insert(it_x, x);
        y_values.insert(y_values.begin() + pos, y);
        needs_update = true;
    }

    const vector<double>& getPolynomial() {
        if (needs_update || current_polynomial.empty()) {
            computeDividedDifferences();
            
            int n = x_values.size();
            current_polynomial = {divided_differences[0][0]};
            vector<double> accumulated_term = {1.0};

            for (int i = 1; i < n; i++) {
                vector<double> factor = {-x_values[i-1], 1.0};
                accumulated_term = multiplyPolynomials(accumulated_term, factor);

                vector<double> current_term;
                for (double coef : accumulated_term) {
                    current_term.push_back(coef * divided_differences[0][i]);
                }
                current_polynomial = addPolynomials(current_polynomial, current_term);
            }
            
            needs_update = false;
        }
        return current_polynomial;
    }

    double evaluate(double x) {
        const vector<double>& poly = getPolynomial();
        double result = 0.0;
        for (int i = poly.size() - 1; i >= 0; i--) {
            result = result * x + poly[i];
        }
        return result;
    }

    void printPolynomial() {
        const vector<double>& poly = getPolynomial();
        int degree = poly.size() - 1;
        bool first_term = true;

        cout << "P(x) = ";
        if (poly.empty()) {
            cout << "0" << endl;
            return;
        }

        for (int i = degree; i >= 0; i--) {
            if (abs(poly[i]) < 1e-10) continue;

            if (!first_term) {
                cout << (poly[i] > 0 ? " + " : " - ");
            } else {
                if (poly[i] < 0) cout << "-";
                first_term = false;
            }

            double coef_abs = abs(poly[i]);
            if (i == 0) {
                cout << coef_abs;
            } else {
                if (coef_abs != 1.0 || i == 0) cout << coef_abs;
                if (coef_abs != 1.0 && i != 0) cout << "*";
                cout << "x";
                if (i > 1) cout << "^" << i;
            }
        }
        cout << endl;
    }

    void printDividedDifferencesTable() {
        if (needs_update) {
            computeDividedDifferences();
        }

        cout << "\nDivided Differences Table:\n";
        cout << fixed << setprecision(6);
        for (size_t i = 0; i < divided_differences.size(); i++) {
            cout << "x = " << x_values[i] << ": ";
            for (size_t j = 0; j < divided_differences[i].size() - i; j++) {
                cout << setw(12) << divided_differences[i][j] << " ";
            }
            cout << endl;
        }
    }
};

void displayMenu() {
    cout << "\nNewton Interpolation Menu\n";
    cout << "1. Add a new point\n";
    cout << "2. Display current polynomial\n";
    cout << "3. Evaluate polynomial at a point\n";
    cout << "4. Display divided differences table\n";
    cout << "5. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    NewtonInterpolator interpolator;
    int choice;
    double x, y;

    cout << "Newton Interpolation Method (Dynamic Version)\n";
    cout << "--------------------------------------------\n";

    // Initial points input
    int n;
    cout << "Enter the number of initial points: ";
    cin >> n;

    cout << "Enter the x and y values for each point:\n";
    for (int i = 0; i < n; i++) {
        cout << "x[" << i << "]: ";
        cin >> x;
        cout << "y[" << i << "]: ";
        cin >> y;
        interpolator.addPoint(x, y);
    }

    // Interactive menu
    do {
        displayMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter new point (x y): ";
                cin >> x >> y;
                interpolator.addPoint(x, y);
                cout << "Point added successfully.\n";
                break;
            case 2:
                cout << "\nCurrent polynomial:\n";
                interpolator.printPolynomial();
                break;
            case 3:
                cout << "Enter x value to evaluate: ";
                cin >> x;
                cout << "P(" << x << ") = " << interpolator.evaluate(x) << endl;
                break;
            case 4:
                interpolator.printDividedDifferencesTable();
                break;
            case 5:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 5);

    return 0;
}
