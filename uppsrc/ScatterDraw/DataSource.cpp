#include <plugin/Eigen/Eigen.h>
using namespace Eigen;

#include "ScatterDraw.h"

namespace Upp {


#define Membercall(fun)	(this->*fun)

double DataSource::Min(Getdatafun getdata, int64& id) {
	double minVal = -DOUBLE_NULL;
	for (int64 i = 0; i < GetCount(); ++i) {
		double d = Membercall(getdata)(i);
		if (!IsNull(d) && minVal > d) {
			minVal = d;
			id = i;
		}
	}
	if (minVal == -DOUBLE_NULL)
		return Null;
	return minVal;		
}
 
double DataSource::Max(Getdatafun getdata, int64& id) {
	double maxVal = DOUBLE_NULL;
	for (int64 i = 0; i < GetCount(); ++i) {
		double d = Membercall(getdata)(i);
		if (!IsNull(d) && maxVal < d) {
			maxVal = d;
			id = i;
		}
	}
	if (maxVal == DOUBLE_NULL)
		return Null;
	return maxVal;
}

double DataSource::Avg(Getdatafun getdata) {
	double ret = 0;
	int count = 0;
	for (int64 i = 0; i < GetCount(); ++i) {
		double d = Membercall(getdata)(i);
		if (!IsNull(d)) {
			ret += d;
			count++;
		}
	}
	if (count == 0) 
		return Null;
	return ret/count;
}

double DataSource::IsSorted(Getdatafun getdata) {
	int64 num = GetCount();
	if (num == 0)
		return false;
	if (num == 1)
		return 1;
	for (int i = 1; i < num; ++i) {
		if (Membercall(getdata)(i) < Membercall(getdata)(i - 1))
			return false;
	}
	return true;
}

double DataSource::Variance(Getdatafun getdata, double avg) {
	if (IsNull(avg))
		avg = Avg(getdata);
	if (IsNull(avg))
		return Null;
	double ret = 0;
	int count = 0;
	for (int64 i = 0; i < GetCount(); ++i) {
		double d = Membercall(getdata)(i);
		if (!IsNull(d)) {
			d -= avg;
			ret += d*d;
			count++;
		}
	}
	if (count <= 0)
		return Null;
	return ret/(count - 1);
}

Vector<int64> DataSource::Envelope(Getdatafun getdataY, Getdatafun getdataX, double width, bool (*fun)(double a, double b)) {
	Vector<int64> ret;
	double width_2 = width/2.;
		
	for (int i = 0; i < GetCount(); ++i) {
		double y = Membercall(getdataY)(i);
		double x = Membercall(getdataX)(i);
		if (IsNull(x) || IsNull(y))
			continue;
		int numComparisons = 0;
		for (int j = i-1; j >= 0; --j) {
			double ynext = Membercall(getdataY)(j);
			double xnext = Membercall(getdataX)(j);
			if (IsNull(xnext) || IsNull(ynext))
				continue;
			if ((x - xnext) > width_2)
				break;
			if (!fun(y, ynext)) {
				numComparisons = Null;
				break;
			}
			numComparisons++;
		}
		if (IsNull(numComparisons))
			continue;
		for (int j = i+1; j < GetCount(); ++j) {
			double ynext = Membercall(getdataY)(j);
			double xnext = Membercall(getdataX)(j);
			if (IsNull(xnext) || IsNull(ynext))
				continue;
			if ((xnext - x) > width_2)
				break;
			if (!fun(y, ynext)) {
				numComparisons = Null;
				break;
			}
			numComparisons++;
		}
		if (IsNull(numComparisons))
			continue;
		if (numComparisons > 2) {
			if (!ret.IsEmpty()) {
				int64 prev_i = ret[ret.GetCount() - 1];
				if (Membercall(getdataX)(prev_i) != x)
					ret << i;
			} else 
				ret << i;
		}
	}
	return ret;
}

bool GreaterEqualThan(double a, double b) {return a >= b;}
bool LowerEqualThan(double a, double b) {return a <= b;}

Vector<int64> DataSource::UpperEnvelope(Getdatafun getdataY, Getdatafun getdataX, double width) {return Envelope(getdataY, getdataX, width, GreaterEqualThan);}
Vector<int64> DataSource::LowerEnvelope(Getdatafun getdataY, Getdatafun getdataX, double width) {return Envelope(getdataY, getdataX, width, LowerEqualThan);}

Vector<Pointf> DataSource::MovingAverage(Getdatafun getdataY, Getdatafun getdataX, double width) {
	Vector<Pointf> ret;
	double width_2 = width/2.;
	
	for (int i = 0; i < GetCount(); ++i) {
		double y = Membercall(getdataY)(i);
		double x = Membercall(getdataX)(i);
		if (IsNull(x) || IsNull(y))
			continue;
		int numAvg = 1;
		double sum = y;
		int j;
		for (j = i-1; j >= 0; --j) {
			double ynext = Membercall(getdataY)(j);
			double xnext = Membercall(getdataX)(j);
			if (IsNull(xnext) || IsNull(ynext))
				continue;
			if ((x - xnext) > width_2)
				break;
			sum += ynext;
			numAvg++;
		}
		if (j < 0)
			continue;
		for (j = i+1; j < GetCount(); ++j) {
			double ynext = Membercall(getdataY)(j);
			double xnext = Membercall(getdataX)(j);
			if (IsNull(xnext))
				continue;
			if ((xnext - x) > width_2) 
				break;
			if (IsNull(ynext))
				continue;
			sum += ynext;
			numAvg++;
		}
		if (j == GetCount())
			continue;
		ret << Pointf(x, sum/numAvg);
	}
	return ret;	
}

Vector<Pointf> DataSource::SectorAverage(Getdatafun getdataY, Getdatafun getdataX, double width) {
	Vector<Pointf> ret;
	
	for (int i = 0; i < GetCount();) {
		double y = Membercall(getdataY)(i);
		double x = Membercall(getdataX)(i);
		if (IsNull(x) || IsNull(y))
			continue;
		
		int numAvg = 1;
		double sum = y;
		double sumX = x;
		int j;
		for (j = i+1; j < GetCount(); ++j) {
			double ynext = Membercall(getdataY)(j);
			double xnext = Membercall(getdataX)(j);
			if (IsNull(xnext))
				continue;
			if ((xnext - x) > width) {
				--j;
				break;
			}
			if (IsNull(ynext))
				continue;
			sumX += xnext;
			sum += ynext;
			numAvg++;
		}
		ret << Pointf(sumX/numAvg, sum/numAvg);
		if (j == GetCount())
			break;
		i = j+1;
	}
	return ret;	
}

void DataSource::ZeroCrossing(Getdatafun getdataY, Getdatafun getdataX, bool ascending, bool descending, 
							  Vector<double> &zeros, Vector<int64> &ids) {
	zeros.Clear();
	ids.Clear();
	
	double y_prev, x_prev;
	int i0;
	for (i0 = 0; i0 < GetCount(); ++i0) {
		y_prev = Membercall(getdataY)(i0);
		x_prev = Membercall(getdataX)(i0);
		if (!IsNull(x_prev) && !IsNull(y_prev))
			break;
	}
	for (int i = i0; i < GetCount(); ++i) {
		double y = Membercall(getdataY)(i);
		double x = Membercall(getdataX)(i);
		if (IsNull(x) || IsNull(y))
			continue;
		
		if (((y >= 0 && y_prev < 0) && ascending) || ((y <= 0 && y_prev > 0) && descending)) {
			ids << i;
			zeros << (x_prev - (x - x_prev)*y_prev/(y - y_prev));
		}
		x_prev = x;
		y_prev = y;
	}
}

double DataSource::StdDev(Getdatafun getdata, double avg) {
	double var = Variance(getdata, avg);
	return IsNull(var) ? Null : sqrt(var);
}

double DataSource::SinEstim_Amplitude(double avg) {
	return sqrt(2.*VarianceY(avg));
}

bool DataSource::SinEstim_FreqPhase(double &frequency, double &phase, double avg) {
	if (GetCount() < 4)
		return false;
	if (IsNull(avg))
		avg = AvgY();
	int64 firstId;
	for (firstId = 0; firstId < GetCount(); ++firstId) 
		if (!IsNull(x(firstId)) && !IsNull(y(firstId)))
			break;
	bool firstIsToPositive = (y(firstId) - avg) < 0;
	bool isPossitive = !firstIsToPositive;
	double T = 0;
	int numT = 0;
	double lastZero = Null;
	double firstZero;
	firstId++;
	for (int64 id = firstId; id < GetCount(); ++id) {
		if (IsNull(x(id)) || IsNull(y(id)))
			continue;
		if (((y(id) - avg) > 0) != isPossitive) {
			isPossitive = !isPossitive;
			double zero = x(id-1) - (y(id-1) - avg)*(x(id) - x(id-1))/(y(id) - y(id-1));
			if (IsNull(lastZero)) 
				firstZero = zero;
			else {
				T += zero - lastZero;
				numT++;
			}
			lastZero = zero;
		}
	}
	if (T == 0 || numT == 0)
		return false;
	T = 2*T/numT;
	frequency = 2*M_PI/T;			
	phase = -frequency*firstZero;
	if (!firstIsToPositive)
		phase += M_PI;
	return true;
}

	
double CArray::znFixed(int n, int64 id) {
	if (n == 0)
		return zData[id];
	NEVER();
	return Null;
}


Vector<Pointf> DataSource::FFT(Getdatafun getdata, double tSample, bool frequency, int type, bool window) {
	int numData = int(GetCount());
    VectorXd timebuf(numData);
    int num = 0;
    for (int i = 0; i < numData; ++i) {
        double data = Membercall(getdata)(i);
        if (!IsNull(data)) {
			timebuf[i] = Membercall(getdata)(i);
			num++;
        }
    }
    Vector<Pointf> res;	
    if (num < 3)
        return res;
    timebuf.resize(num);
    
    double windowSum = 0;
    if (window) {
	    for (int i = 0; i < numData; ++i) {
	        double windowDat = 0.54 - 0.46*cos(2*M_PI*i/numData);
	        windowSum += windowDat;
	    	timebuf[i] *= windowDat;	// Hamming window
	    }
	} else
		windowSum = numData;
	
    VectorXcd freqbuf;
    try {
	    Eigen::FFT<double> fft;
	    fft.SetFlag(fft.HalfSpectrum);
	    fft.fwd(freqbuf, timebuf);
    } catch(...) {
        return res;
    }
    if (frequency) {
    	for (int i = 0; i < int(freqbuf.size()); ++i) {    
    		double xdata = i/(tSample*numData);
		
			switch (type) {
			case T_PHASE:	res << Pointf(xdata, std::arg(freqbuf[i]));				break;
			case T_FFT:		res << Pointf(xdata, 2*std::abs(freqbuf[i])/windowSum);	break;
			case T_PSD:		res << Pointf(xdata, 2*sqr(std::abs(freqbuf[i]))/(windowSum/tSample));
			}
    	}
    } else {
        for (int i = int(freqbuf.size()) - 1; i > 0; --i) {    
    		double xdata = (tSample*numData)/i;
		
			switch (type) {
			case T_PHASE:	res << Pointf(xdata, std::arg(freqbuf[i]));				break;
			case T_FFT:		res << Pointf(xdata, 2*std::abs(freqbuf[i])/windowSum);	break;
			case T_PSD:		res << Pointf(xdata, 2*sqr(std::abs(freqbuf[i]))/(windowSum/tSample));
			}
    	}
    }
    return res;
}


}