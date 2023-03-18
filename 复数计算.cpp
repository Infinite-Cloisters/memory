#include<iostream>
class ImgNum {
	friend std::ostream& operator << (std::ostream& cout, ImgNum& Res) {
		if (Res.Real) {
			cout << Res.Real;
			if (Res.Img) {
				if (Res.Img > 0)	cout << '+';
				cout << Res.Img << 'i';
			}
			else if (Res.Img < 0) cout << Res.Img << 'i';
		}
		else if (Res.Img)	cout << Res.Img << 'i';
		return cout;
	}
	friend std::istream& operator >> (std::istream& cin, ImgNum& Res) {
		std::cout << "Real:";
		cin >> Res.Real;
		std::cout << "Img:";
		cin >> Res.Img;
		return cin;
	}
public:
	ImgNum(long double R = 0, long double I = 0) :Real(R), Img(I) {};
	long double Real;
	long double Img;
	void RealAdd(long double R = 1) { Real += R; }
	void ImgAdd(long double I = 1) { Img += I; }
	ImgNum GetConjugate(void) {
		return { Real,-Img };
	}
	ImgNum operator+(long double N) {
		return { Real + N,Img + N };
	}
	ImgNum operator+(ImgNum& Res) {
		return { Real + Res.Real,Img + Res.Img };
	}
	ImgNum operator-(long double N) {
		return { Real - N,Img - N };
	}
	ImgNum operator-(ImgNum& Res) {
		return { Real - Res.Real,Img - Res.Img };
	}
	ImgNum operator*(long double N) {
		return { Real * N,Img * N };
	}
	ImgNum operator*(ImgNum& Res) {
		return { Real * Res.Real - Img * Res.Img,Real * Res.Img + Img * Res.Real };
	}
	ImgNum operator/(long double N) {
		return { Real / N,Img / N };
	}
	ImgNum operator/(ImgNum& Res) {
		ImgNum Result = Res.GetConjugate();
		return *this * Result / (Res.Real * Res.Real + Res.Img * Res.Img);
	}
};
int main() {
}
