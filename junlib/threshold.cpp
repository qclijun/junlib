#include "threshold.h"



// ���ط�ֵ���ܻҶȣ�ǰ���Ҷ�
std::array<int, 3> otsuStats(const jun::JMatrix& m, int ch){
	const int nCh = m.channels();
	const int SZ = 256;
	int hist[SZ];
	std::memset(hist, 0, SZ * sizeof(int));

	for (int r = 0; r < m.rows(); ++r){
		auto p = m.iter(r, 0);
		for (int c = 0; c < m.cols(); ++c){
			++hist[p[c*nCh + ch]];
		}
	}

	double sum = 0.0; //������
	double csum = 0.0; //��ǰ������
	int n = m.rows()*m.cols();//�����ص�

	int n1 = 0;// �Ҷ�С�ڷ�ֵ��ǰ���������ص���
	int n2 = 0;// �Ҷȴ��ڷ�ֵ���󾰣������ص���
	double fmax = 0.0;//�����䷽��
	double sb = 0.0;//��䷽��
	double m1 = 0.0; //ǰ��ƽ���Ҷ�
	double m2 = 0.0;//��ƽ���Ҷ�

	int t = 0;//��ֵ

	for (int i = 0; i < SZ; ++i){
		//n += hist[i];
		sum += (double)i*hist[i];
	}

	for (int i = 0; i < SZ - 1; ++i){
		n1 += hist[i];
		if (n1 == 0) continue;
		n2 = n - n1;
		if (n2 == 0) break;
		csum += double(i)*hist[i];
		m1 = csum / n1;
		m2 = (sum - csum) / n2;
		sb = (m1 - m2)*(m1 - m2)*n1*n2;
		if (sb > fmax){
			fmax = sb;
			t = i;
		}

	}
	return{ { t, n, n1 } };

}

jun::BinaryJMatrix otsuThreshold(const jun::JMatrix& src){
	jun::BinaryJMatrix binary(src.rows(), src.cols()); //binaryImage

	const int nCh = src.channels();
	int* threshold = new int[nCh];

	//hi_value:
	//true---���ڷ�ֵ������(��ɫ)Ϊ����
	bool* hi_values = new bool[nCh];
	bool goodFlag = false;//�õĻ��֣��������غ�ǰ��������Ŀ���ܴ�

	int best_hi_dist = 0;
	bool best_hi_value = false;
	int best_hi_index = 0;

	for (int ch = 0; ch < nCh; ++ch){
		std::array<int, 3> arr = otsuStats(src, ch);
		int t = arr[0], n = arr[1], n1 = arr[2];
		threshold[ch] = t;
		if (n1 == 0 || n - n1 == 0) //channel is empty
			continue;
		bool hi_value = n1 < 0.5*n;
		if (n1 < 0.25*n){
			goodFlag = true;
			hi_values[ch] = true;
		}
		else if (n1 > 0.75*n){
			goodFlag = true;
			hi_values[ch] = false;
		}
		else{
			int hi_dist = hi_value ? (n - n1) : n1; //����������Ŀ
			if (best_hi_dist>hi_dist){
				best_hi_dist = hi_dist;
				best_hi_value = hi_value;
				best_hi_index = ch;
			}
		}
	}

	if (!goodFlag){
		hi_values[best_hi_index] = best_hi_value;
	}

	for (int r = 0; r < src.rows(); ++r){
		for (int c = 0; c < src.cols(); ++c){
			auto p = src.iter(r, c);
			bool white_result = true;
			for (int ch = 0; ch < nCh; ++ch){
				if (hi_values[ch] == (p[ch] < threshold[ch])){
					white_result = false;
					break;
				}
			}
			if (white_result)
				binary(r, c) = 0;
			else
				binary(r, c) =1;
		}
	}



	delete[] threshold;
	delete[] hi_values;
	return binary;
}

