#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 100
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLINE 4096   /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8      /*maximum number of client connections */
enum type
{
    COURSE,
    ACCOUNT,
    REGISTRATION

};
enum dayOfWeek
{
    EMPTY,
    EMPTY1,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    SUNDAY
};
enum sessionOfDay
{
    EMPTY2,
    AM,
    PM
};
typedef struct course
{
    int courseId;
    char code[MAX_LENGTH];
    char name[MAX_LENGTH];
    enum dayOfWeek day;
    enum sessionOfDay sessionStart;
    enum sessionOfDay sessionEnd;
    int startAt;
    int endAt;
    char week[MAX_LENGTH];
    char position[MAX_LENGTH];
} course;

typedef struct registration
{
    int accountId;
    int courseId[MAX_LENGTH];
    int count;
} registration;
typedef struct account
{
    int accountId;
    char password[MAX_LENGTH];
} account;
union Data
{
    struct course course;
    struct registration registration;
    struct account account;
};
struct LinkedList
{
    enum type type;
    union Data data;
    struct LinkedList *next;
};

typedef struct LinkedList *node; //Từ giờ dùng kiểu dữ liệu LinkedList có thể thay bằng node cho ngắn gọn

node accHead, regisHead, courseHead;
int connfd;

node CreateNode(union Data value)
{
    node temp;                                      // declare a node
    temp = (node)malloc(sizeof(struct LinkedList)); // Cấp phát vùng nhớ dùng malloc()
    temp->next = NULL;                              // Cho next trỏ tới NULL
    temp->data = value;                             // Gán giá trị cho Node
    return temp;                                    //Trả về node mới đã có giá trị
}

node AddTail(node head, union Data value)
{
    node temp, p;             // Khai báo 2 node tạm temp và p
    temp = CreateNode(value); //Gọi hàm createNode để khởi tạo node temp có next trỏ tới NULL và giá trị là value
    if (head == NULL)
    {
        head = temp; //Nếu linked list đang trống thì Node temp là head luôn
    }
    else
    {
        p = head; // Khởi tạo p trỏ tới head
        while (p->next != NULL)
        {
            p = p->next; //Duyệt danh sách liên kết đến cuối. Node cuối là node có next = NULL
        }
        p->next = temp; //Gán next của thằng cuối = temp. Khi đó temp sẽ là thằng cuối(temp->next = NULL mà)
    }
    return head;
}

node AddHead(node head, union Data value)
{
    node temp = CreateNode(value); // Khởi tạo node temp với data = value
    if (head == NULL)
    {
        head = temp; // //Nếu linked list đang trống thì Node temp là head luôn
    }
    else
    {
        temp->next = head; // Trỏ next của temp = head hiện tại
        head = temp;       // Đổi head hiện tại = temp(Vì temp bây giờ là head mới mà)
    }
    return head;
}

node AddAt(node head, union Data value, int position)
{
    if (position == 0 || head == NULL)
    {
        head = AddHead(head, value); // Nếu vị trí chèn là 0, tức là thêm vào đầu
    }
    else
    {
        // Bắt đầu tìm vị trí cần chèn. Ta sẽ dùng k để đếm cho vị trí
        int k = 1;
        node p = head;
        while (p != NULL && k != position)
        {
            p = p->next;
            ++k;
        }

        if (k != position)
        {
            // Nếu duyệt hết danh sách lk rồi mà vẫn chưa đến vị trí cần chèn, ta sẽ mặc định chèn cuối
            // Nếu bạn không muốn chèn, hãy thông báo vị trí chèn không hợp lệ
            head = AddTail(head, value);
            // printf("Vi tri chen vuot qua vi tri cuoi cung!\n");
        }
        else
        {
            node temp = CreateNode(value);
            temp->next = p->next;
            p->next = temp;
        }
    }
    return head;
}

node DelHead(node head)
{
    if (head == NULL)
    {
        printf("\nCha co gi de xoa het!");
    }
    else
    {
        head = head->next;
    }
    return head;
}

node DelTail(node head)
{
    if (head == NULL || head->next == NULL)
    {
        return DelHead(head);
    }
    node p = head;
    while (p->next->next != NULL)
    {
        p = p->next;
    }
    p->next = p->next->next; // Cho next bằng NULL
    // Hoặc viết p->next = NULL cũng được
    return head;
}

node DelAt(node head, int position)
{
    if (position == 0 || head == NULL || head->next == NULL)
    {
        head = DelHead(head); // Nếu vị trí chèn là 0, tức là thêm vào đầu
    }
    else
    {
        // Bắt đầu tìm vị trí cần chèn. Ta sẽ dùng k để đếm cho vị trí
        int k = 1;
        node p = head;
        while (p->next->next != NULL && k != position)
        {
            p = p->next;
            ++k;
        }

        if (k != position)
        {
            // Nếu duyệt hết danh sách lk rồi mà vẫn chưa đến vị trí cần chèn, ta sẽ mặc định xóa cuối
            // Nếu bạn không muốn xóa, hãy thông báo vị trí xóa không hợp lệ
            head = DelTail(head);
            // printf("Vi tri xoa vuot qua vi tri cuoi cung!\n");
        }
        else
        {
            p->next = p->next->next;
        }
    }
    return head;
}

union Data Get(node head, int index)
{
    int k = 0;
    node p = head;
    while (p != NULL && k != index)
    {
        p = p->next;
    }
    return p->data;
}

// int Search(node head, Data value){
//     int position = 0;
//     for(node p = head; p != NULL; p = p->next){
//         if(p->data.id == value.id && strcmp(p->data.password, value.password)==0){
//             return position;
//         }
//         ++position;
//     }
//     return -1;
// }
// node Find(node head, int key){
//     for(node p = head; p != NULL; p = p->next){
//         if(p->data.id == key){
//             return p;
//         }
//     }
//     return NULL;
// }
node FindAccount(node head, int id)
{
    for (node p = head; p != NULL; p = p->next)
    {
        if (p->data.account.accountId == id)
        {
            return p;
        }
    }
    return NULL;
}
node FindRegistration(node head, int id)
{
    for (node p = head; p != NULL; p = p->next)
    {
        if (p->data.registration.accountId == id)
        {
            return p;
        }
    }
    return NULL;
}
node FindCourse(node head, int id)
{
    for (node p = head; p != NULL; p = p->next)
    {
        if (p->data.course.courseId == id)
        {
            return p;
        }
    }
    return NULL;
}
node Find(node head, int id)
{
    switch (head->type)
    {
    case ACCOUNT:
        return FindAccount(head, id);
    case REGISTRATION:
        return FindRegistration(head, id);
    case COURSE:
        return FindCourse(head, id);
    default:
        printf("ERROR: Cannot find item because unknown type\n");
        break;
    }
}

node InitHead()
{
    node head;
    head = NULL;
    return head;
}

int Length(node head)
{
    int length = 0;
    for (node p = head; p != NULL; p = p->next)
    {
        ++length;
    }
    return length;
}
char *enumDayToString(enum dayOfWeek day)
{
    switch (day)
    {
    case MONDAY:
        return "Monday";
    case TUESDAY:
        return "Tuesday";
    case WEDNESDAY:
        return "Wednesday";
    case THURSDAY:
        return "Thursday";
    case FRIDAY:
        return "Friday";
    case SATURDAY:
        return "Saturday";
    case SUNDAY:
        return "Sunday";
    default:
        return "[Unknown day]";
    }
}
char *enumSessionToString(enum dayOfWeek day)
{
    switch (day)
    {
    case AM:
        return "Morning";
    case PM:
        return "Afternoon";
    default:
        return "[Unknown session]";
    }
}
void printCourse(course c)
{
    printf("%s\t|%-20s\t|%10s|%10s|%d - %d |%s\t|%s\n", c.code, c.name, enumDayToString(c.day),
           enumSessionToString(c.sessionStart), c.startAt, c.endAt, c.week, c.position);
}
void Traverser(node head)
{
    printf("\n");
    course course;
    if (head->type == ACCOUNT)
    {
        for (node p = head; p != NULL; p = p->next)
        {
            printf("%5d %s\n", p->data.account.accountId, p->data.account.password);
        }
    }

    else if (head->type == REGISTRATION)
    {
        for (node p = head; p != NULL; p = p->next)
        {
            printf("%5d ", p->data.registration.accountId);
            for (int i = 0; i <= p->data.registration.count; i++)
            {
                printf("%5d ", p->data.registration.courseId[i]);
            }
        }
    }

    else if (head->type == COURSE)
    {
        for (node p = head; p != NULL; p = p->next)
        {
            course = p->data.course;
            printCourse(course);
        }
    }
}

node readfile(char *filename)
{
    FILE *input = NULL;
    union Data data;
    node registration;
    node head, curNode;
    int count, courseId;
    char c;
    input = fopen(filename, "rt");
    if (input == NULL)
    {
        return 0;
    }
    if (filename == "User-account.txt")
    {
        head = InitHead();
        while (fscanf(input, "%d%s", &data.account.accountId, data.account.password) != EOF)
        {
            head = AddTail(head, data);
        }
        head->type = ACCOUNT;
    }

    else if (filename == "student_registration.txt")
    {

        head = InitHead(REGISTRATION);
        fscanf(input, "%d %d", &data.registration.accountId, &courseId);
        data.registration.count = 0;
        data.registration.courseId[0] = courseId;
        head = AddTail(head, data);
        head->type = REGISTRATION;
        while (fscanf(input, "%d %d", &data.registration.accountId, &courseId) != EOF)
        {
            curNode = Find(head, data.registration.accountId);
            if (curNode == NULL)
            {
                data.registration.count = 0;
                data.registration.courseId[0] = courseId;
                head = AddTail(head, data);
            }
            else
            {
                curNode->data.registration.count += 1;
                curNode->data.registration.courseId[curNode->data.registration.count] = courseId;
            }
        }
    }

    else if (filename == "course_schedule.txt")
    {
        // 119747 IT3080 Computer Network 523,526,22,25-31,33-40,TC-502;
        head = InitHead();
        while (fscanf(input, "%d", &data.course.courseId) != EOF)
        {
            fscanf(input, "%s", data.course.code);
            fscanf(input, "%[^0-9]s", data.course.name);
            fscanf(input, "%c", &c);
            data.course.day = c - '0';
            fscanf(input, "%c", &c);
            data.course.sessionStart = c - '0';
            fscanf(input, "%c", &c);
            data.course.startAt = c - '0';
            fscanf(input, "%c%c", &c, &c);
            fscanf(input, "%c", &c);
            data.course.sessionEnd = c - '0';
            fscanf(input, "%c", &c);
            data.course.endAt = c - '0';
            fscanf(input, "%c", &c);
            fscanf(input, "%[^A-Z]s", data.course.week);
            data.course.week[strlen(data.course.week) - 1] = '\0';
            fscanf(input, "%[^;]s", data.course.position);
            fscanf(input, "%c", &c);
            head = AddTail(head, data);
        }
        head->type = COURSE;
    }

    fclose(input);
    return head;
}

// int checkDay(node head, int day){
//     for(int i=0; i <=head->data.registration.count;i++ ) {
//     node cur= Find(courseHead, head->data.registration.courseId);
//     if(cur==NULL) continue;
//     else
//     }
// }

int isSubjectInDay(int courseId, int day)
{
    node cur = Find(courseHead, courseId);

    if (cur == NULL)
        return 0;
    course c = cur->data.course;
    if (cur->data.course.day == day)
    {
        return 1;
    }
    return 0;
}

void getDaySchedule(node loginAcc, int day)
{
    course c;
    node cur = Find(regisHead, loginAcc->data.account.accountId);
    if (cur == NULL)
    {
        printf("Course registration not found");
        return;
    }

    registration regis = cur->data.registration;
    printf("\nYour schedule in %s: \n", enumDayToString(day));
    printf("%s\t|%-20s\t|%10s|%-10s|%6s|%s\t\t|%s\n", " Code", "Course", "Week Day", " AM/PM", "Period", " Week", "Room");
    for (int i = 0; i <= regis.count; i++)
    {
        if (isSubjectInDay(regis.courseId[i], day))
        {
            c = Find(courseHead, regis.courseId[i])->data.course;
            printCourse(c);
        };
    }
}
// node PrintRoomByPeriod(node loginAcc, int period){
//     node cur =  Find(regisHead, loginAcc->data.account.accountId);
//     course course[MAX_LENGTH]= cur->data.registration.courseId;
//     course c= course[0];
//     for(int i=1; i<=12;i++ ){
//         if(c.startAt>=period&& c.endAt>=period){
//             printf("%2d|%s\t|%s\t|%s\t|%s|\n")
//         }
//     }

//     for(node p = head; p != NULL; p = p->next){
//         if(p->data.course.startAt>=period&& p->data.course.endAt>=period){
//             printf("%2d|%s\t|%s\t|%s\t|%s|\n")
//         }
//     }
//     return NULL;
// }
void getWeekSchedule(node loginAcc)
{
    course c;
    node cur = Find(regisHead, loginAcc->data.account.accountId);
    if (cur == NULL)
    {
        printf("Course registration not found");
    }
    printf(" |Monday\t|Tuesday\t|Wednesday\t|Thursday\t|Friday\t|Saturday\t|Sunday\t|\n");
    // registration regis = cur->data.registration;

    // for(int i=1; i<=12;i++ ){
    //     for(int j=0; j<= regis.count; j++){
    //     c = FindCourse(courseHead, regis.courseId[j])->data.course;
    //     if(c.startAt>=i&& c.endAt>=i){
    //         printf("%2d|%s\t|%s\t|%s\t|%s|\n",i,c.position);
    //     }
    //     }
    // }
}

char *sendToClient(char *buf)
{
    char *recvline = malloc(sizeof(char *) * MAXLINE);
    if (send(connfd, buf, MAXLINE, 0) < 0)
    {
        perror("Read error");
        exit(1);
    }
    if (recv(connfd, recvline, MAXLINE, 0) < 0)
    {
        perror("Read error");
        exit(1);
    }
    recvline[strlen(recvline) - 1] = '\0';
    return recvline;
}

int main()
{

    int choice = 0;
    account sv;
    memset(&sv, 0, sizeof(account));
    node loginAcc, rNode = NULL;
    char weekday[MAX_LENGTH];
    // enum dayOfWeek day;
    int day, isCheck = 0;
    registration registration;
    accHead = readfile("User-account.txt");
    regisHead = readfile("student_registration.txt");
    courseHead = readfile("course_schedule.txt");

    int listenfd, n;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;

    //creation of the socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    //preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);
    printf("%s\n", "Server running...waiting for connections.");

    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
    printf("%s\n", "Received request...");

    while (choice != 4)
    {
        strcpy(buf, "\n\tMENU DEMONSTRATION");
        strcat(buf, "\n\t------------------------------");
        strcat(buf, "\n\n\t 1. Login");
        strcat(buf, "\n\t 2. Read schedule");
        strcat(buf, "\n\t 3. Display the busy schedule");
        strcat(buf, "\n\t 4. EXIT");
        strcat(buf, "\n\n Enter Your Choice: ");
        choice = atoi(sendToClient(buf));
        switch (choice)
        {
        case 1:
            strcpy(buf,"YOU SELECTED OPTION 1\n");
            strcat(buf,"Enter your id: ");
            sv.accountId = atoi(sendToClient(buf));
            loginAcc = Find(accHead, sv.accountId);
            if (loginAcc == NULL)
            {
                strcpy(buf,"Invalid id");
                sendToClient(buf);
                break;
            }
            strcpy(buf,"Enter your password: ");
            strcpy(sv.password, sendToClient(buf));
            if (strcmp(loginAcc->data.account.password, sv.password))
            {
                strcpy(buf,"Error password.\n");
                sendToClient(buf);
            }
            else
            {
                strcpy(buf,"Login success.\n");
                sendToClient(buf);
                isCheck = 1;
            }
            break;
        case 2:
            if (isCheck == 0)
            {
                strcpy(buf,"\nPlease login first");
                sendToClient(buf);
                break;
            }

            strcpy(buf,"\nRead schedule");
            strcat(buf,"\nSelect a week day: ");
            day= atoi(sendToClient(buf));
            getDaySchedule(loginAcc, day);

            break;
        case 3:
            if (isCheck == 0)
            {
                printf("\nPlease login first");
                break;
            }
            loginAcc = Find(accHead, 20184077);
            strcpy(buf,"\nDisplay the busy schedule\n\n");
            sendToClient(buf);
            getWeekSchedule(loginAcc);
            break;
        case 4:
            strcpy(buf,"\nExit");
            sendToClient(buf);
            exit(0);
        default:
            strcpy(buf,"\nINVALID SELECTION...Please try again");
            sendToClient(buf);
        }
    }
    //close listening socket
    close(listenfd);
}
