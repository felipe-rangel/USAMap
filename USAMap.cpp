#define _USE_MATH_DEFINES
#include "library.h"
double inf = INFINITY;
int const size = 30000;
string states[52] = { "AL", "AK", "AZ", "AR", "CA", "CO", "CT", "DC", "DE", "FL", "GA", "HI", "ID", "IL", "IN", "IA", "KS", "KY", "LA", "ME", "MD", "MA", "MI", "MN", "MS", "MO", "MT", "NE", "NV", "NH", "NJ", "NM", "NY", "NC", "ND", "OH", "OK", "OR", "PA", "PR", "RI", "SC", "SD", "TN", "TX", "UT", "VT", "VA", "WA", "WV", "WI", "WY" };
struct place{
	int nCode, pop, rCode;
	string state, name;
	double area, lat, lon, dist;
	place(int nc, string st, string na, int p, double a, double la, double lo, int rc, double d){
		nCode = nc; state = st;
		name = na; pop = p;
		area = a; lat = la;
		lon = lo; rCode = rc;
		dist = d;
	}
};
struct hashLink{
	place *data;
	hashLink *next;
	hashLink(place *p, hashLink * n = NULL){
		data = p;
		next = n;
	}
};
class HashTable{
protected:
	hashLink * data[size];
public:
	HashTable(){
		for (int i = 0; i<size; i++){
			data[i] = NULL;
		}
	}
	unsigned int HashN(string s);
	void addPlace(place *p);
	void addData(string file);
	void printPlace(place *p);
	int findStatePlace(string st, string na);
	void findAllPlaces(string na);
};
unsigned int HashTable::HashN(string s){
	unsigned int val = 1759619;
	for (int i = 0; i<s.length(); i++){
		val = val * 787 + s[i];
	}
	return val%size;
}
void HashTable::addPlace(place *p){
	string st_na = (p->state) + (p->name);
	int pos = HashN(st_na);
	data[pos] = new hashLink(p, data[pos]);
}
void HashTable::addData(string file){
	ifstream fin(file);
	int nc, p, rc;
	string block, st, na, temp;
	double a, la, lo, d;
	while (true){
		if (fin.eof()){
			break;
		}
		fin >> block;
		while ((fin.peek() <= 48 || fin.peek() >= 57) && !fin.eof()){
			if (fin.get() == ' '&&fin.peek() == ' '){
				break;
			}
			fin >> temp;
			block = block + " " + temp;
		}
		fin >> p >> a >> la;
		temp.clear();
		if (!fin.eof()){
			while (fin.peek() != '\n'){
				temp += fin.get();
			}
			lo = stod(temp.substr(0, 10));
			rc = stoi(temp.substr(11, 15));
			d = stod(temp.substr(16));
		}
		nc = stoi(block.substr(0, 8));
		st = block.substr(8, 2);
		na = block.substr(10);
		if (!fin.eof()){
			addPlace(new place(nc, st, na, p, a, la, lo, rc, d));
		}
	}
	fin.close();
}
void HashTable::printPlace(place *p){
	cout << p->nCode << " " << p->state << " " << p->name << " " << p->pop << " " << p->area << " "
		<< p->lat << " " << p->lon << " " << p->rCode << " " << p->dist << endl;
}
int HashTable::findStatePlace(string st, string na){
	string st_na = st + na;
	int pos = HashN(st_na);
	hashLink * curr = data[pos];
	if (data[pos] == NULL){
		return -1;
	}
	else{
		while (curr != NULL){
			if (curr->data->state == st&&curr->data->name == na){
				printPlace(curr->data);
				return curr->data->rCode;
			}
			curr = curr->next;
		}
		return -1;
	}
}
void HashTable::findAllPlaces(string na){
	bool flag = false;
	for (int i = 0; i<52; i++){
		if (findStatePlace(states[i], na)){
			flag = true;
		}
	}
	if (!flag){
		cout << "Place Not in Data" << endl;
	}
}
struct intersection{
	double lon, lat, dist;
	string state, name;
	intersection(double lo, double la, double d, string s, string n){
		lon = lo;	dist = d;
		lat = la;	state = s;
		name = n;
	}
};
struct road{
	string name, type;
	int from, to;
	double length;
	road(string n, string ty, int f, int t, double l){
		name = n;	type = ty;
		from = f;	to = t;
		length = l;
	}
};
struct node{
	intersection *inter;
	vector <road *> exits;
	double startDistance;
	bool visited;
	int position;
	node(intersection *i, int p){
		inter = i;
		visited = false;
		startDistance = inf;
		position = p;
	}
	void addRoad(road *r){
		exits.push_back(r);
	}
};
class MinQueue{
protected:
	vector<node *> queue;
public:
	MinQueue(){};
	void enqueue(node * loc);
	node * dequeue();
	bool empty();
	bool inQueue(node * loc);
	void printQ();
};
void MinQueue::enqueue(node * loc){
	queue.push_back(loc);
}
node * MinQueue::dequeue(){
	node * minNode = queue[0];
	int minPos = 0;
	for (int i = 0; i<queue.size(); i++){
		if (minNode->startDistance>queue[i]->startDistance){
			minNode = queue[i];
			minPos = i;
		}
	}
	queue.erase(queue.begin() + minPos);
	return minNode;
}
bool MinQueue::empty(){
	return queue.empty();
}
bool MinQueue::inQueue(node * loc){
	for (int i = 0; i<queue.size(); i++){
		if (loc == queue[i]){
			return true;
		}
	}
	return false;
}
void MinQueue::printQ(){
	for (int i = 0; i<queue.size(); i++){
		cout << queue[i]->inter->name << " " << queue[i]->position << endl;
	}
	cout << endl;
}
class Graph{
protected:
	vector <node *> data;
	int pos = 0;
public:
	Graph(){}
	void addIData(string file);
	void addRData(string file);
	bool setPos(int p);
	string compassDirection(int from, int to);
	double getShortestPathLength(int from, int to);
	vector <pair<double, double>> printShortestPath(int from, int to, double dist);
};
void Graph::addIData(string file){
	ifstream fin(file);
	int position = 0;
	double la, lo, d;
	string na, st, temp;
	while (true){
		if (fin.eof()){
			break;
		}
		fin >> lo >> la >> d >> st >> na;
		while (fin.peek() != '\n'&&!fin.eof()){
			fin >> temp;
			na = na + " " + temp;
		}
		if (!fin.eof()){
			data.push_back(new node(new intersection(lo, la, d, st, na), position));
			position++;
		}
	}
	fin.close();
}
void Graph::addRData(string file){
	ifstream fin(file);
	int fr, to;
	double l;
	string na, ty;
	while (true){
		if (fin.eof()){
			break;
		}
		fin >> na >> ty >> fr >> to >> l;
		if (!fin.eof()){
			road * curr_from = new road(na, ty, fr, to, l);
			road * curr_to = new road(na, ty, to, fr, l);
			data[fr]->addRoad(curr_from);
			data[to]->addRoad(curr_to);
		}
	}
	fin.close();
}
bool Graph::setPos(int p){
	if (p>0 && p <= data.size()){
		pos = p;
		return true;
	}
	else{
		cout << "Incorrect Location" << endl;
		return false;
	}
}
string Graph::compassDirection(int from, int to){
	double from_lat = data[from]->inter->lat;
	double from_lon = data[from]->inter->lon;
	double to_lat = data[to]->inter->lat;
	double to_lon = data[to]->inter->lon;
	double rad = atan2((to_lat - from_lat), (to_lon - from_lon));
	int compassDegrees;
	if (rad<0){
		compassDegrees = (int)((rad + 2 * M_PI)*(180.0 / M_PI));
	}
	else{
		compassDegrees = (int)(rad*(180.0 / M_PI));
	}
	if (((compassDegrees>345) && (compassDegrees <= 360)) || (compassDegrees >= 0) && (compassDegrees <= 15)){
		return "E";
	}
	else if ((compassDegrees >= 15) && (compassDegrees <= 75)){
		return "NE";
	}
	else if ((compassDegrees>75) && (compassDegrees<105)){
		return "N";
	}
	else if ((compassDegrees >= 105) && (compassDegrees <= 165)){
		return "NW";
	}
	else if ((compassDegrees>165) && (compassDegrees<195)){
		return "W";
	}
	else if ((compassDegrees >= 195) && (compassDegrees <= 255)){
		return "SW";
	}
	else if ((compassDegrees>255) && (compassDegrees<285)){
		return "S";
	}
	else if ((compassDegrees >= 285) && (compassDegrees <= 345)){
		return "SE";
	}
	return "N/A";
}
double Graph::getShortestPathLength(int from, int to){
	MinQueue Q;
	node * curr=NULL, *next=NULL;
	if (!setPos(to)){
		return -1;
	}
	if (!setPos(from)){
		return -1;
	}
	node * end = data[to];
	data[pos]->startDistance = 0;
	Q.enqueue(data[pos]);
	while (!Q.empty()){
		curr = Q.dequeue();
		curr->visited = true;
		if (curr == end){
			break;
		}
		for (int i = 0; i<curr->exits.size(); i++){
			road * r = curr->exits[i];
			if (r != NULL){
				next = data[r->to];
				if (!(next->visited) && !Q.inQueue(next)){
					next->startDistance = curr->startDistance + r->length;
					Q.enqueue(next);
				}
				else if (curr->startDistance + r->length<next->startDistance){
					next->startDistance = curr->startDistance + r->length;
				}
			}
		}
	}
	if (curr != end){
			return -1;
	}else{
		return curr->startDistance;
	}
	return -1;
}
bool doubleSame(double x, double y, double diff = 0.001f){
	if (fabs(x - y)<diff){
		return true;
	}
	else{
		return false;
	}
}
vector <pair<double, double>> Graph::printShortestPath(int from, int to, double dist){
	vector <string> directions;
	vector <road *> mainRoads;
	vector <pair<double, double>> path_lat_lon;
	double printDist = 0;
	if (!setPos(from)){
		cout << "No Path Between Two Locations" << endl;
		return path_lat_lon;
	}
	cout << "Shortest Path Length from " << data[pos]->inter->name << ", " << data[pos]->inter->state << " " << pos << " to ";
	if (!setPos(to)){
		cout << "No Path Between Two Locations" << endl;
		return path_lat_lon;
	}
	cout << data[pos]->inter->name << ", " << data[pos]->inter->state << " " << pos << ": " << dist << " miles." << endl;
	node * curr = data[pos];
	if (dist == -1){
		cout << "No Path Between Two Locations" << endl;
		return path_lat_lon;
	}else{
		while (dist>0){
			for (int i = 0; i<curr->exits.size(); i++){
				road * r = curr->exits[i];
				node * before = data[r->to];
				double difference = curr->startDistance - r->length;
				if (doubleSame(difference, before->startDistance)){
					if (mainRoads.empty()){
						mainRoads.push_back(r);
						printDist = r->length;
						path_lat_lon.push_back(make_pair(curr->inter->lat, curr->inter->lon));
					}else if (mainRoads.back()->name != r->name){
						string s = "Go " + compassDirection(r->to, mainRoads.back()->from) + " on " + mainRoads.back()->name + " for " + to_string(printDist) + " miles to " + data[mainRoads.back()->from]->inter->name;
						directions.push_back(s);
						mainRoads.push_back(r);
						printDist = r->length;
						path_lat_lon.push_back(make_pair(curr->inter->lat, curr->inter->lon));
					}
					else{
						printDist += r->length;
						path_lat_lon.push_back(make_pair(curr->inter->lat, curr->inter->lon));
					}
					curr = before;
					dist = (int)((dist - r->length)*1000.0) / 1000.0;
					if (dist <= 0){
						string s = "Go " + compassDirection(r->to, mainRoads.back()->from) + " on " + mainRoads.back()->name + " for " + to_string(printDist) + " miles to " + data[mainRoads.back()->from]->inter->name;
						directions.push_back(s);
					}
					break;
				}
			}
		}
	}
	path_lat_lon.push_back(make_pair(curr->inter->lat, curr->inter->lon));
	for (int i = directions.size() - 1; i >= 0; i--){
		cout << directions[i] << endl;
	}
	return path_lat_lon;
}
class GMap{
protected:
	string dataFile="";
public:
	void selectMap(string coverageFile,double stLat, double stLo, double endLat, double endLo);
	void drawMap(vector<pair<double,double>> path_lo_lat);
};
void GMap::selectMap(string coverageFile, double stLat, double stLo, double endLat, double endLo){
	ifstream fin(coverageFile);
	double minDistLat=inf, minDistLo=inf, currDistLat, currDistLo, maxLat, minLat, maxLo, minLo;
	string currFile;
	while (true){
		if (fin.eof()){
			break;
		}
		fin >> maxLat >> minLat >> minLo >> maxLo >> currFile;
		currDistLat = maxLat - minLat;
		currDistLo = maxLo - minLo;
		if ((stLat<=maxLat&&minLat<=stLat) && (endLat<=maxLat&&minLat<=endLat) 
			&& (stLo<=maxLo&&minLo<=stLo) && (endLo<=maxLo&&minLo<=endLo)
			&&(currDistLat<minDistLat)&&(currDistLo<minDistLo)){
				dataFile = currFile;
				minDistLat = currDistLat;
				minDistLo = currDistLo;
		}
	}
}
void GMap::drawMap(vector<pair<double, double>> path_lo_lat){
	if (dataFile == ""){
		cout << "No Data File Found to Draw Map" << endl;
		return;
	}
	ifstream fin(dataFile, ios::in | ios::binary);
	string s;
	int rows=0, cols=0, pixelByte, leftLon, topLat, minP, maxP, waterP, num;
	double pixelDeg;
	for (int i = 0; i < 9; i++){
		fin >> s >> num;
		if (s == "rows"){
			rows = num;
		}else if (s == "columns"){
			cols = num;
		}else if (s == "bytesperpixel"){
			pixelByte = num;
		}else if (s == "secondsperpixel"){
			pixelDeg = (double)num;
		}else if (s == "leftlongseconds"){
			leftLon = num;
		}else if (s == "toplatseconds"){
			topLat= num;
		}else if (s == "min"){
			minP = num;
		}else if (s == "max"){
			maxP = num;
		}else if (s == "specialval"){
			waterP = num;
		}
	}
	make_window(cols, rows);
	short int values[800];
	for (int dataRow = 0; dataRow < rows; dataRow++){
		fin.seekg(dataRow*cols*pixelByte, ios::beg);
		fin.read((char *)values, sizeof(values));
		if (fin.gcount() != sizeof(values))
			break;
		for (int dataCol = 0; dataCol < cols; dataCol++){
			if (values[dataCol] == waterP)
				set_pixel_color(dataCol, dataRow, color::dark_blue);
			else if (minP <= values[dataCol] && values[dataCol] < 100)
				set_pixel_color(dataCol, dataRow, color::light_green);
			else if (100 <= values[dataCol] && values[dataCol] < 300)
				set_pixel_color(dataCol, dataRow, color::green);
			else if (300 <= values[dataCol] && values[dataCol] < 900)
				set_pixel_color(dataCol, dataRow, color::dark_green);
			else if (900 <= values[dataCol] && values[dataCol] < 1700)
				set_pixel_color(dataCol, dataRow, color::brown);
			else if (1700 <= values[dataCol] && values[dataCol] < 2700)
				set_pixel_color(dataCol, dataRow, color::grey);
			else if (2700 <= values[dataCol] && values[dataCol] <= 4048)
				set_pixel_color(dataCol, dataRow, color::white);
		}
	}
	fin.close();
	set_pen_color(color::red);
	topLat = topLat / 3600.0;
	leftLon = leftLon / 3600.0;
	pixelDeg = 1.0/(pixelDeg/3600.0);
	set_pen_width(5);
	draw_point(-1.0*(leftLon - path_lo_lat.back().second)*pixelDeg, (topLat - path_lo_lat.back().first)*pixelDeg);
	for (int i = path_lo_lat.size() - 2; i >= 0; i--){
		draw_to(-1.0*(leftLon - path_lo_lat[i].second)*pixelDeg, (topLat - path_lo_lat[i].first)*pixelDeg);
		wait(0.001); //To See the Path Being Drawn
	}
}
void main(){
	string placeN1, state1, placeN2, state2;
	int start, end;
	HashTable H;
	Graph G;
	GMap M;
	cout << "Loading Data" << endl;
	H.addData("named-places.txt");
	cout << "Named-Places Data Loaded" << endl;
	G.addIData("intersections.txt");
	cout << "Intersections Data Loaded" << endl;
	G.addRData("connections.txt");
	cout << "Connections Data Loaded" << endl;
	cout << "Please input place name of start:" << endl;
	getline(cin, placeN1);
	cout << "Please input state abbreviation of start:" << endl;
	cin >> state1;
	int rCode1 = H.findStatePlace(state1, placeN1);
	if (rCode1 != -1){
		start = rCode1;
	}else{
		cout << "Place Not in Data" << endl;
		return;
	}
	cout << "Please input place name of end:" << endl;
	cin.ignore();
	getline(cin, placeN2);
	cout << "Please input state abbreviation of end:" << endl;
	cin >> state2;
	int rCode2 = H.findStatePlace(state2, placeN2);
	if (rCode2 != -1){
		end = rCode2;
	}
	else{
		cout << "Place Not in Data" << endl;
		return;
	}
	vector<pair<double, double>> path_lo_lat=G.printShortestPath(start, end, G.getShortestPathLength(start, end));
	if (!path_lo_lat.empty()){
		M.selectMap("coverage.txt", path_lo_lat.back().first, path_lo_lat.back().second, path_lo_lat.front().first, path_lo_lat.front().second);
		M.drawMap(path_lo_lat);
	}
}
