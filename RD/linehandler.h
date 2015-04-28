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
//using namespace cv;

class Line{
public:
	Line(cv::Point pt1, cv::Point pt2, int degree) : pt1(pt1), pt2(pt2), degree(degree){}
	cv::Point pt1, pt2;
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
	void addLines(vector<cv::Vec2f>& lines, cv::Mat & image){
		std::vector<cv::Vec2f>::const_iterator it = lines.begin();
		while (it != lines.end()) {
			float rho = (*it)[0];   // first element is distance rho
			float theta = (*it)[1]; // second element is angle theta
			int degree = theta * (180 / PI);
			cv::Point pt1(rho / cos(theta), 0);
			cv::Point pt2((rho - image.rows*sin(theta)) / cos(theta), image.rows);
			addLine(Line(pt1, pt2, degree));
			++it;
		}
	}
	int Matches(){
		return (groups.size());
	}
	void drawNBestLines(cv::Mat &image, const int N){
		vector<Line> resultlines = findNBestLines(N);
		vector<Line>::iterator it1 = resultlines.begin();
		while (it1 != resultlines.end()){
			line(image, (*it1).pt1, (*it1).pt2, cv::Scalar(255), 2);
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

	void fillBestLines(cv::Mat &image){
		vector<Line> lines = findNBestLines(2);
		vector<cv::Point> points;
		cv::Point i;
		if (lines.size() > 1 && intersection(lines[0], lines[1], i)){
			points.push_back(i);
			for each (Line l in lines){
				points.push_back(lowerPoint(l));
			}
		}
		else{
			for each (Line l in lines){
				points.push_back(l.pt1);
				points.push_back(l.pt2);
			}
		}
		fill(image, points);

	}

	// Fills the polygon defined by the given points, on top of the given image
	// color is optional, white by default
	void fill(cv::Mat &image, vector<cv::Point> points, cv::Scalar color = cv::Scalar(100)){
		vector<vector<cv::Point> > allPoints;
		allPoints.push_back(points);
		cv::fillPoly(image, allPoints, color);

	}

	// Finds the intersection of two lines, or returns false.
	bool intersection(Line line1, Line line2, cv::Point &intersection)
	{
		cv::Point o1 = line1.pt1;
		cv::Point p1 = line1.pt2;
		cv::Point o2 = line2.pt1;
		cv::Point p2 = line2.pt2;

		cv::Point x = o2 - o1;
		cv::Point d1 = p1 - o1;
		cv::Point d2 = p2 - o2;

		float cross = d1.x*d2.y - d1.y*d2.x;
		if (abs(cross) < /*EPS*/1e-8)
			return false;

		double t1 = (x.x * d2.y - x.y * d2.x) / cross;
		intersection = o1 + d1 * t1;
		return true;
	}

	// Returns the lower point of the line
	cv::Point lowerPoint(Line l){
		if (l.pt1.y > l.pt2.y){
			return l.pt1;
		}
		else{
			return l.pt2;
		}
	}
private:
	vector<Group> groups;
};







#endif