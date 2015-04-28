#if !defined LINEHANDLER
#define LINEHANDLER

/*
Deze klasse bewaart alle lijnen met een bepaalde hoek.
En groepeert deze lijnen in groepen als ze ongeveer dezelfde hoek hebben.
Uit deze groepen kunnen dan de gemiddelde lijn berekend worden.
*/
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
using namespace std;
using namespace cv;

class Line{
public:
	Line(Point pt1, Point pt2, int degree) : pt1(pt1), pt2(pt2), degree(degree){}
	Point pt1, pt2;
	int degree;
};

class Group {
public:
	Group() : degree(0), aantal(0) {}
	void addLine(Line l){
		if (aantal == 0){
			degree = l.degree;
		}
		else{
			degree = (degree + l.degree) / 2;
		}
		aantal++;
		lines.push_back(l);
	}

	Line getMedianLine(){
		vector<Line>::iterator it = lines.begin();
		Line max = *it;
		it++;
		while (it != lines.end()){
			if (abs(degree - max.degree) > abs(degree - (*it).degree)){
				max = *it;
			}
			it++;
		}
		return max;
	}
	int degree;
	int aantal;
private:
	vector<Line> lines;

};


class LineHandler{
public:
	int interval = 10;
	void clear(){
		groups.clear();

	}
	void addLine(Line l){
		if ((l.degree > 0 && l.degree < 80) || (l.degree > 100 && l.degree < 140)) {
			vector<Group>::iterator it = groups.begin();
			bool notadded = true;
			while (it != groups.end() && notadded){
				if (((*it).degree - interval) <= l.degree && ((*it).degree + interval) >= l.degree){
					(*it).addLine(l);
					notadded = false;
				}
				else{
					it++;
				}
			}
			if (it == groups.end() && notadded){
				Group g = Group();
				g.addLine(l);
				groups.push_back(g);
			}
		}
	}
	void addLines(vector<Vec2f>& lines, Mat & image){
		std::vector<Vec2f>::const_iterator it = lines.begin();
		while (it != lines.end()) {
			float rho = (*it)[0];   // first element is distance rho
			float theta = (*it)[1]; // second element is angle theta
			int degree = theta * (180 / PI);
			Point pt1(rho / cos(theta), 0);
			Point pt2((rho - image.rows*sin(theta)) / cos(theta), image.rows);
			addLine(Line(pt1, pt2, degree));
			++it;
		}
	}
	int Matches(){
		return (groups.size());
	}
	void drawNBestLines(Mat &image, const int N){
		vector<Line> resultlines = findNBestLines(N);
		vector<Line>::iterator it1 = resultlines.begin();
		while (it1 != resultlines.end()){
			line(image, (*it1).pt1, (*it1).pt2, Scalar(255), 2);
			std::cout << (*it1).pt1 << (*it1).pt2 << (*it1).degree << std::endl;
			it1++;
		}
	}
	vector<Line> findNBestLines(const int N){
		vector<Line> lines;
		if (groups.size() > N){
			vector<Group>::iterator it = groups.begin();
			Group *best= new Group[N];
			for (int i = 0; i < N; i++){
				best[i] = (*it);
				it++;
			}
			while (it != groups.end()){
				int lowest = 0;
				for (int i = 1; i<N; i++){
					if (best[i].aantal<best[lowest].aantal){
						lowest = i;
					}
				}
				if (best[lowest].aantal <(*it).aantal){
					best[lowest] = (*it);
				}
				it++;
			}
			for (int i = 0; i < N; i++){
				lines.push_back(best[i].getMedianLine());
			}
			
		}
		else{
			vector<Group>::iterator it = groups.begin();
			while (it != groups.end()){
				lines.push_back((*it).getMedianLine());
				it++;
			}
		}
		return lines;

	}
private:
	vector<Group> groups;
};







#endif