#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <functional>
#include <chrono>
#include <iomanip>

using namespace std;

const int MAX_NAME_LEN = 50;
const string DATABASE_FILE = "customers.dat";
const int MIN_CUSTOMERS = 1000;

struct Customer {
    string firstName;
    string lastName;
    int customerID;
    Customer* next;
};

struct BSTNode {
    Customer data;
    BSTNode* left;
    BSTNode* right;
};

enum Color { RED, BLACK };

struct RBTNode {
    Customer data;
    RBTNode* left;
    RBTNode* right;
    RBTNode* parent;
    Color color;
};

// Global variables
Customer* customerList = nullptr;
BSTNode* bstRoot = nullptr;
RBTNode* rbtRoot = nullptr;
int customerCount = 0;

// Function prototypes
void generateSampleData();
void saveToFile();
void loadFromFile();
void addCustomerToList(const Customer& c);
void insertBST(const Customer& c);
void insertRBT(const Customer& c);
void fixRBTInsert(RBTNode* node);
void leftRotateRBT(RBTNode* x);
void rightRotateRBT(RBTNode* y);
Customer* searchList(const string& lastName);
Customer* searchBST(const string& lastName);
Customer* searchBSTHelper(BSTNode* root, const string& lastName);
Customer* searchRBT(const string& lastName);
Customer* searchRBTHelper(RBTNode* root, const string& lastName);
void printCustomer(Customer* c);
void freeMemory();
void performanceTest();
void displayMenu();
void printSearchResults(const string& lastName);

int compareNames(const string& a, const string& b) {
    string aLower = a;
    string bLower = b;
    transform(aLower.begin(), aLower.end(), aLower.begin(), ::tolower);
    transform(bLower.begin(), bLower.end(), bLower.begin(), ::tolower);
    return aLower.compare(bLower);
}

void generateSampleData() {
    const vector<string> firstNames = {"John", "Jane", "Michael", "Emily", "David", "Sarah", 
                                     "Robert", "Jennifer", "William", "Lisa", "James", 
                                     "Jessica", "Daniel", "Amy", "Christopher"};
    const vector<string> lastNames = {"Smith", "Johnson", "Williams", "Brown", "Jones", 
                                    "Miller", "Davis", "Garcia", "Rodriguez", "Wilson", 
                                    "Martinez", "Anderson", "Taylor", "Thomas", "Lee"};
    
    srand(time(nullptr));
    for (int i = 0; i < MIN_CUSTOMERS; i++) {
        Customer c;
        c.firstName = firstNames[rand() % firstNames.size()];
        c.lastName = lastNames[rand() % lastNames.size()];
        c.customerID = 1000 + i;
        c.next = nullptr;
        
        addCustomerToList(c);
        insertBST(c);
        insertRBT(c);
        customerCount++;
    }
    saveToFile();
}

void addCustomerToList(const Customer& c) {
    Customer* newCustomer = new Customer;
    *newCustomer = c;
    newCustomer->next = customerList;
    customerList = newCustomer;
}

void insertBST(const Customer& c) {
    BSTNode** current = &bstRoot;
    BSTNode* parent = nullptr;
    
    while (*current != nullptr) {
        parent = *current;
        if (compareNames(c.lastName, (*current)->data.lastName) < 0) {
            current = &(*current)->left;
        } else {
            current = &(*current)->right;
        }
    }
    
    *current = new BSTNode;
    (*current)->data = c;
    (*current)->left = nullptr;
    (*current)->right = nullptr;
}

void insertRBT(const Customer& c) {
    RBTNode* new_node = new RBTNode;
    new_node->data = c;
    new_node->left = nullptr;
    new_node->right = nullptr;
    new_node->color = RED;
    
    RBTNode* parent = nullptr;
    RBTNode* current = rbtRoot;
    
    while (current != nullptr) {
        parent = current;
        if (compareNames(c.lastName, current->data.lastName) < 0) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    
    new_node->parent = parent;
    if (parent == nullptr) {
        rbtRoot = new_node;
    } else if (compareNames(c.lastName, parent->data.lastName) < 0) {
        parent->left = new_node;
    } else {
        parent->right = new_node;
    }
    
    fixRBTInsert(new_node);
}

void fixRBTInsert(RBTNode* node) {
    while (node != rbtRoot && node->parent->color == RED) {
        if (node->parent == node->parent->parent->left) {
            RBTNode* uncle = node->parent->parent->right;
            if (uncle != nullptr && uncle->color == RED) {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    leftRotateRBT(node);
                }
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                rightRotateRBT(node->parent->parent);
            }
        } else {
            RBTNode* uncle = node->parent->parent->left;
            if (uncle != nullptr && uncle->color == RED) {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    rightRotateRBT(node);
                }
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                leftRotateRBT(node->parent->parent);
            }
        }
    }
    rbtRoot->color = BLACK;
}

void leftRotateRBT(RBTNode* x) {
    RBTNode* y = x->right;
    x->right = y->left;
    if (y->left != nullptr) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr) {
        rbtRoot = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void rightRotateRBT(RBTNode* y) {
    RBTNode* x = y->left;
    y->left = x->right;
    if (x->right != nullptr) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == nullptr) {
        rbtRoot = x;
    } else if (y == y->parent->right) {
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
}

Customer* searchList(const string& lastName) {
    Customer* current = customerList;
    while (current != nullptr) {
        if (compareNames(lastName, current->lastName) == 0) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

Customer* searchBSTHelper(BSTNode* root, const string& lastName) {
    if (root == nullptr) return nullptr;
    
    int cmp = compareNames(lastName, root->data.lastName);
    if (cmp == 0) return &root->data;
    if (cmp < 0) return searchBSTHelper(root->left, lastName);
    return searchBSTHelper(root->right, lastName);
}

Customer* searchBST(const string& lastName) {
    return searchBSTHelper(bstRoot, lastName);
}

Customer* searchRBTHelper(RBTNode* root, const string& lastName) {
    if (root == nullptr) return nullptr;
    
    int cmp = compareNames(lastName, root->data.lastName);
    if (cmp == 0) return &root->data;
    if (cmp < 0) return searchRBTHelper(root->left, lastName);
    return searchRBTHelper(root->right, lastName);
}

Customer* searchRBT(const string& lastName) {
    return searchRBTHelper(rbtRoot, lastName);
}

void printCustomer(Customer* c) {
    if (c == nullptr) {
        cout << "Customer not found.\n";
        return;
    }
    cout << "First Name: " << c->firstName << "\n";
    cout << "Last Name: " << c->lastName << "\n";
    cout << "Customer ID: " << c->customerID << "\n\n";
}

void saveToFile() {
    ofstream outFile(DATABASE_FILE, ios::binary);
    if (!outFile) {
        cerr << "Error opening file for writing\n";
        return;
    }
    
    Customer* current = customerList;
    while (current != nullptr) {
        size_t len = current->firstName.size();
        outFile.write(reinterpret_cast<char*>(&len), sizeof(len));
        outFile.write(current->firstName.c_str(), len);
        
        len = current->lastName.size();
        outFile.write(reinterpret_cast<char*>(&len), sizeof(len));
        outFile.write(current->lastName.c_str(), len);
        
        outFile.write(reinterpret_cast<char*>(&current->customerID), sizeof(current->customerID));
        current = current->next;
    }
    
    outFile.close();
}

void loadFromFile() {
    ifstream inFile(DATABASE_FILE, ios::binary);
    if (!inFile) {
        cout << "No existing database found. Generating sample data...\n";
        generateSampleData();
        return;
    }
    
    while (true) {
        Customer c;
        size_t len;
        
        if (!inFile.read(reinterpret_cast<char*>(&len), sizeof(len))) break;
        char* buffer = new char[len + 1];
        inFile.read(buffer, len);
        buffer[len] = '\0';
        c.firstName = buffer;
        delete[] buffer;
        
        if (!inFile.read(reinterpret_cast<char*>(&len), sizeof(len))) break;
        buffer = new char[len + 1];
        inFile.read(buffer, len);
        buffer[len] = '\0';
        c.lastName = buffer;
        delete[] buffer;
        
        if (!inFile.read(reinterpret_cast<char*>(&c.customerID), sizeof(c.customerID))) break;
        
        c.next = nullptr;
        addCustomerToList(c);
        insertBST(c);
        insertRBT(c);
        customerCount++;
    }
    
    inFile.close();
    
    if (customerCount < MIN_CUSTOMERS) {
        cout << "Database has only " << customerCount << " customers. Generating more...\n";
        generateSampleData();
    }
}

void freeMemory() {
    // Free linked list
    Customer* current = customerList;
    while (current != nullptr) {
        Customer* temp = current;
        current = current->next;
        delete temp;
    }
    customerList = nullptr;
    
    // Free BST
    function<void(BSTNode*)> freeBST = [&](BSTNode* root) {
        if (root == nullptr) return;
        freeBST(root->left);
        freeBST(root->right);
        delete root;
    };
    freeBST(bstRoot);
    bstRoot = nullptr;
    
    // Free RBT
    function<void(RBTNode*)> freeRBT = [&](RBTNode* root) {
        if (root == nullptr) return;
        freeRBT(root->left);
        freeRBT(root->right);
        delete root;
    };
    freeRBT(rbtRoot);
    rbtRoot = nullptr;
}

void performanceTest() {
    if (customerCount == 0) {
        cout << "No customers in database.\n";
        return;
    }

    const int TEST_COUNT = 1000;
    vector<string> testNames;
    
    for (int i = 0; i < TEST_COUNT; i++) {
        int randomIndex = rand() % customerCount;
        Customer* temp = customerList;
        for (int j = 0; j < randomIndex && temp != nullptr; j++) {
            temp = temp->next;
        }
        if (temp != nullptr) {
            testNames.push_back(temp->lastName);
        }
    }

    cout << "\nPerformance Test - Testing " << TEST_COUNT << " searches\n";
    
    using namespace chrono;
    high_resolution_clock::time_point start, end;
    double listTime, bstTime, rbtTime;
    
    // Linked List search
    start = high_resolution_clock::now();
    for (const auto& name : testNames) {
        searchList(name);
    }
    end = high_resolution_clock::now();
    listTime = duration_cast<duration<double>>(end - start).count();
    
    // BST search
    start = high_resolution_clock::now();
    for (const auto& name : testNames) {
        searchBST(name);
    }
    end = high_resolution_clock::now();
    bstTime = duration_cast<duration<double>>(end - start).count();
    
    // RBT search
    start = high_resolution_clock::now();
    for (const auto& name : testNames) {
        searchRBT(name);
    }
    end = high_resolution_clock::now();
    rbtTime = duration_cast<duration<double>>(end - start).count();

    // Display results
    cout << fixed << setprecision(6);
    cout << "Linked List Search: " << listTime << " seconds\n";
    cout << "BST Search: " << bstTime << " seconds\n";
    cout << "RBT Search: " << rbtTime << " seconds\n\n";

    // Determine fastest
    double fastestTime = min({listTime, bstTime, rbtTime});
    string fastestMethod;
    
    if (fastestTime == listTime) fastestMethod = "Linked List";
    else if (fastestTime == bstTime) fastestMethod = "BST";
    else fastestMethod = "RBT";

    cout << "--> Fastest method: " << fastestMethod << "\n\n";

    // Calculate and display percentage differences
    cout << fixed << setprecision(2);
    cout << "Performance Comparison:\n";
    
    if (fastestMethod != "Linked List") {
        double slowerPercent = (listTime - fastestTime)/fastestTime * 100;
        cout << "- Linked List is " << slowerPercent << "% slower than " << fastestMethod << endl;
    }
    
    if (fastestMethod != "BST") {
        double slowerPercent = (bstTime - fastestTime)/fastestTime * 100;
        cout << "- BST is " << slowerPercent << "% slower than " << fastestMethod << endl;
    }
    
    if (fastestMethod != "RBT") {
        double slowerPercent = (rbtTime - fastestTime)/fastestTime * 100;
        cout << "- RBT is " << slowerPercent << "% slower than " << fastestMethod << endl;
    }
}

void printSearchResults(const string& lastName) {
    cout << "\nSearch Results for '" << lastName << "':\n";
    
    vector<Customer*> listMatches, bstMatches, rbtMatches;
    
    // Search linked list
    Customer* current = customerList;
    while (current != nullptr) {
        if (compareNames(lastName, current->lastName) == 0) {
            listMatches.push_back(current);
        }
        current = current->next;
    }
    
    // Search BST
    function<void(BSTNode*)> collectBST = [&](BSTNode* node) {
        if (!node) return;
        if (compareNames(lastName, node->data.lastName) == 0) {
            bstMatches.push_back(&node->data);
        }
        collectBST(node->left);
        collectBST(node->right);
    };
    collectBST(bstRoot);
    
    // Search RBT
    function<void(RBTNode*)> collectRBT = [&](RBTNode* node) {
        if (!node) return;
        if (compareNames(lastName, node->data.lastName) == 0) {
            rbtMatches.push_back(&node->data);
        }
        collectRBT(node->left);
        collectRBT(node->right);
    };
    collectRBT(rbtRoot);
    
    // Display results
    cout << "1. Linked List (" << listMatches.size() << " matches):\n";
    for (auto c : listMatches) printCustomer(c);
    
    cout << "2. BST (" << bstMatches.size() << " matches):\n";
    for (auto c : bstMatches) printCustomer(c);
    
    cout << "3. RBT (" << rbtMatches.size() << " matches):\n";
    for (auto c : rbtMatches) printCustomer(c);
}

void displayMenu() {
    cout << "\nCustomer Database System\n";
    cout << "1. Search Customer\n";
    cout << "2. Performance Test\n";
    cout << "3. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    loadFromFile();
    cout << "Customer Database Loaded. Total customers: " << customerCount << "\n";
    
    while (true) {
        displayMenu();
        int choice;
        cin >> choice;
        cin.ignore(); // Clear newline
        
        switch (choice) {
            case 1: {
                string searchName;
                cout << "Enter last name to search: ";
                getline(cin, searchName);
                printSearchResults(searchName);
                break;
            }
            case 2:
                performanceTest();
                break;
            case 3:
                freeMemory();
                cout << "Exiting program. Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
}

//Code written by Ray Clement (UPM) & Fitri Aiman (UPM)