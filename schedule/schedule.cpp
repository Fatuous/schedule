#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class group {
public:
	struct timetable {
		int cabinet = NULL;
		string lesson;
		string type;
	};

	string name;
	int quant;
	int flow;
	timetable predmet[16][6][5];
};

class cab {
public:
	int number; 
	int capacity;
	string type;
	int left[16][6][5]; // Оставшееся количество мест
	int flow[16][6][5]; // Номер потока, которым занята аудитория
	string lesson[16][6][5]; // Предмет, который проходит в данной аудитории
	cab() {
		for (int i = 0; i < 16; i++)
			for (int j = 0; j < 6; j++) 
				for (int k = 0; k < 5; k++)
					flow[i][j][k] = NULL;
	}
	void set_capacity() {
		for (int i = 0; i < 16; i++)
			for (int j = 0; j < 6; j++)
				for (int k = 0; k < 5; k++)
					left[i][j][k] = capacity;
	}
};

class lesson {
public:
	string name;
	int hours;
	string type;
	int flow;
};

int cab_quant() {
	int n = 0;
	string t;
	ifstream fcab("cab.txt");
	while (!fcab.eof()) {
		n++;
		getline(fcab, t);
	}
	fcab.close();
	return n;
}

int group_quant() {
	int n = 0;
	string t;
	ifstream fgr("group.txt");
	while (!fgr.eof()) {
		n++;
		getline(fgr, t);
	}
	fgr.close();
	return n;
}

int lesson_quant() {
	int n = 0, k = 0;
	string t;
	ifstream fless("lessons.txt");
	while (!fless.eof()) {
		getline(fless, t);
		if (t == "\0") k++;
		n++;
	}
	fless.close();
	return n - k*2 - 1;
}

void push_cab(cab *x, int n) {
	ifstream fcab("cab.txt");
	for (int i = 0; i < n; i++) {
		fcab >> x[i].number >> x[i].capacity >> x[i].type;
		x[i].set_capacity();
	}
	fcab.close();
}

void push_group(group *x, int n) {
	ifstream fgr("group.txt");
	for (int i = 0; i < n; i++) {
		fgr >> x[i].name >> x[i].quant >> x[i].flow;
	}
	fgr.close();
}

void push_lessons(lesson *x, int n) {
	ifstream fless("lessons.txt");
	int lesson_flow;
	int i = 0;
	string temp;
	streampos old_pos;
	getline(fless, temp);
	lesson_flow = stoi(temp);
	while (!fless.eof()) {
		old_pos = fless.tellg();
		getline(fless, temp);
		if (temp == "\0") {
			getline(fless, temp);
			lesson_flow = stoi(temp);
		}
		else {
			fless.seekg(old_pos);
			fless >> x[i].name >> x[i].type >> x[i].hours;
			x[i].flow = lesson_flow;
			fless.ignore();
			i++;
		}
	}
	fless.close();
}

void sort_groups(group *x, int n) {
	group temp;
	for (int i = 0; i < n - 1; i++) {
		for (int j = i; j < n; j++) {
			if (x[j].quant > x[i].quant) {
				temp = x[i];
				x[i] = x[j];
				x[j] = temp;
			}
		}
	}
}

void sort_cabs(cab *x, int n) {
	cab temp;
	for (int i = 0; i < n - 1; i++) {
		for (int j = i; j < n; j++) {
			if (x[j].capacity > x[i].capacity) {
				temp = x[i];
				x[i] = x[j];
				x[j] = temp;
			}
		}
	}
}

void sort_lessons(lesson *x, int n) {
	lesson temp;
	for (int i = 0; i < n - 1; i++) {
		for (int j = i; j < n; j++) {
			if (x[j].hours > x[i].hours) {
				temp = x[i];
				x[i] = x[j];
				x[j] = temp;
			}
		}
	}
}

void print(group* groups, int m) {
	string days[6] = { "Понедельник:\n\t", "Вторник:\n\t", "Среда:\n\t", "Четверг:\n\t", "Пятница:\n\t", "Суббота:\n\t" };
	ofstream fout("output.txt");

	for (int u = 0; u < m; u++) {
		fout << "=================================" << groups[u].name << "=================================\n";
		for (int i = 0; i < 16; i++) {
			fout << "-----------------НЕДЕЛЯ " << i + 1 << "-----------------\n";
			for (int j = 0; j < 6; j++) {
				fout << days[j];
				for (int para = 0; para < 5; para++) {
					if (groups[u].predmet[i][j][para].lesson == "Физ-ра")
						fout << "| Физ-ра-пр-ФОК |";
					else if (groups[u].predmet[i][j][para].lesson == "Ин.яз")
						fout << "| Ин.яз-пр-каф |";
					else if (groups[u].predmet[i][j][para].cabinet == NULL)
						fout << "| Нет пары |";
					else
						fout << "| " << groups[u].predmet[i][j][para].lesson << "-" << groups[u].predmet[i][j][para].type << "-" << groups[u].predmet[i][j][para].cabinet << " |";
				}
				fout << "\n";
			}
			fout << "\n";
		}
		fout << "\n\n";
	}
	fout.close();
}

void create(cab *cabs, group *groups, lesson *lessons, int n, int m, int k) {
	int left; // Количество нераспределенных занятий
	int weeks; // Количество недель
	int days; // Количество занятий в неделю
	bool ok = false; // Найдена ли аудитория?
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < k; j++) {
			if (lessons[j].flow != groups[i].flow) continue; // у этой группы есть этот предмет?

			ok = false;
			left = lessons[j].hours / 2;
			if (left > 16) {
				weeks = 16;
				days = left / 16;
			}
			else {
				weeks = left;
				days = 1;
			}
			while (left > 0) {
				int weeks_i = 0;
				while (weeks_i < 16){
					int para_i = 0;
					while (para_i < 5) {
						int days_i = 0;
						while (days_i < 6){
							for (int u = 0; u < n; u++) {
								if (cabs[u].type == lessons[j].type								  // Если тип кабинета == типу занятия,
									&& groups[i].predmet[weeks_i][days_i][para_i].cabinet == NULL // у группы нет занятий на этой паре,
									&& cabs[u].left[weeks_i][days_i][para_i] >= groups[i].quant	  // хватает мест для группы

									&& (((cabs[u].flow[weeks_i][days_i][para_i] == groups[i].flow		  // кабинет занят потоком == потоку текущей группы
											&& cabs[u].lesson[weeks_i][days_i][para_i] == lessons[j].name) // и там проходит такой же предмет
											|| cabs[u].flow[weeks_i][days_i][para_i] == NULL)			  // или кабинет свободен
										|| cabs[u].type == "фок" || cabs[u].type == "каф")) {					// или это занятие на каф или в фоке
										
									cabs[u].flow[weeks_i][days_i][para_i] = groups[i].flow;
									cabs[u].left[weeks_i][days_i][para_i] -= groups[i].quant;
									cabs[u].lesson[weeks_i][days_i][para_i] = lessons[j].name;
									groups[i].predmet[weeks_i][days_i][para_i].cabinet = cabs[u].number;
									groups[i].predmet[weeks_i][days_i][para_i].lesson = lessons[j].name;
									groups[i].predmet[weeks_i][days_i][para_i].type = lessons[j].type;
									left--;
									ok = true;
									break;
								}
							}
							if (left == 0) break;
							if (ok) days_i += 6 / days;
							else days_i++;
						}
						if (left == 0) break;
						if (ok) para_i = 5;
						else para_i++;
					}
					if (left == 0) break;
					if (ok) {
						ok = false; 
						weeks_i += 16 / weeks;
					}
					else weeks_i ++;
				}
			}
	
		}
	}
}

int main()
{
	setlocale(LC_ALL, "Russian");

	int n = cab_quant();
	int m = group_quant();
	int k = lesson_quant();

	cab *cabs = new cab[n]; // массив аудиторий
	group *groups = new group[m]; // массив групп
	lesson *lessons = new lesson[k]; // массив предметов

	push_cab(cabs, n);
	push_group(groups, m);
	push_lessons(lessons, k);

	sort_groups(groups, m);
	sort_cabs(cabs, n);
	sort_lessons(lessons, k);

	create(cabs, groups, lessons, n, m, k);

	print(groups, m);

	return 0;
}