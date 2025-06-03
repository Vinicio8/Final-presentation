#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

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

vector<double> newtonToStandard(const vector<double>& x, const vector<double>& y) {
    int n = x.size();
    vector<vector<double>> table(n, vector<double>(n, 0.0));

    for (int i = 0; i < n; i++) table[i][0] = y[i];
    for (int j = 1; j < n; j++) {
        for (int i = 0; i < n - j; i++) {
            table[i][j] = (table[i + 1][j - 1] - table[i][j - 1]) / (x[i + j] - x[i]);
        }
    }

    vector<double> final_polynomial = {table[0][0]};
    vector<double> accumulated_term = {1.0};

    for (int i = 1; i < n; i++) {
        vector<double> factor = {-x[i - 1], 1.0};
        accumulated_term = multiplyPolynomials(accumulated_term, factor);

        vector<double> current_term;
        for (double coef : accumulated_term) {
            current_term.push_back(coef * table[0][i]);
        }
        final_polynomial = addPolynomials(final_polynomial, current_term);
    }
    return final_polynomial;
}

void printPolynomial(const vector<double>& polynomial) {
    int degree = polynomial.size() - 1;
    bool first_term = true;

    for (int i = degree; i >= 0; i--) {
        if (abs(polynomial[i]) < 1e-10) continue;

        if (!first_term) {
            cout << (polynomial[i] > 0 ? " + " : " - ");
        } else {
            if (polynomial[i] < 0) cout << "-";
            first_term = false;
        }

        double coef_abs = abs(polynomial[i]);
        if (i == 0) {
            cout << coef_abs;
        } else {
            if (coef_abs != 1.0) cout << coef_abs << "*";
            cout << "x";
            if (i > 1) cout << "^" << i;
        }
    }
    cout << endl;
}

int main() {
    cout << "Newton Interpolation Method\n";
    cout << "---------------------------\n";

    int n;
    cout << "Enter the number of points (n): ";
    cin >> n;

    vector<double> x(n), y(n);
    cout << "Enter the x and y values for each point:\n";
    for (int i = 0; i < n; i++) {
        cout << "x[" << i << "]: ";
        cin >> x[i];
        cout << "y[" << i << "]: ";
        cin >> y[i];
    }

    vector<double> newton_poly = newtonToStandard(x, y);

    cout << "\nNewton Polynomial in standard form:\n";
    cout << "P(x) = ";
    printPolynomial(newton_poly);

    return 0;
}
