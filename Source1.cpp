
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <occi.h>
#include <string>
#include <sstream>
#include <cstring>

#include <iomanip>




using oracle::occi::Environment;
using oracle::occi::Connection;


using namespace oracle::occi;
using namespace std;
using std::setw;

struct ShoppingCart {
    int product_id;
    string name;
    double price;
    int quantity;
};
struct Product {
    double price;
    string name;
};

int mainMenu();
int subMenu();
void customerService(Connection* conn, int customerId);
void displayOrderStatus(Connection* conn, int orderId, int customerId); // you write this function
void cancelOrder(Connection* conn, int orderId, int customerId); // you write this function
void createEnvironement(Environment* env);
void openConnection(Environment* env, Connection* conn, string user, string pass, string constr);
void closeConnection(Connection* conn, Environment* env);
void teminateEnvironement(Environment* env);
int customerLogin(Connection* conn, int customerId);
void findProduct(Connection* conn, int product_id, struct Product* product);
int addToCart(Connection* conn, struct ShoppingCart cart[]);
void displayProducts(struct ShoppingCart cart[], int productCount);
int checkout(Connection* conn, struct ShoppingCart cart[], int customerId, int productCount);


int main(void)
{
    int option;
    /* OCCI Variables */
    Environment* env = nullptr;
    Connection* conn = nullptr;
    //Statement* stmt = nullptr;
    //ResultSet* rs = nullptr;

    /* Used Variables */
    string str;
    string user = "dbs311_243zaa36";
    string pass = "14100112";
    string constr = "myoracle12c.senecacollege.ca:1521/oracle12c";


    try {
        // create environement and Open the connction
        env = Environment::createEnvironment(Environment::DEFAULT);
        conn = env->createConnection(user, pass, constr);

        int customerId = 0;

        do {

            option = mainMenu();
            switch (option) {

            case 1:

                cout << "Enter the customer ID: ";
                cin >> customerId;

                if (customerLogin(conn, customerId) == 1) {
                    //call customerService()
                    customerService(conn, customerId);
                }
                else {
                    cout << "customer does not exist." << endl;
                }
                break;
            case 0:
                cout << "Good bye!..." << endl;
                break;


            }
        } while (option != 0);

        env->terminateConnection(conn);
        Environment::terminateEnvironment(env);

    }

    catch (SQLException& sqlExcp) {
        cout << "error";
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }

    return 0;
}

int mainMenu() {
    int option = 1;
    do {
        // diplay the menu options
        cout << "******************** Main Menu ********************" << endl;
        cout << "1)	Login" << endl;
        cout << "0)	Exit" << endl;
        // read an option value

        if (option < 0 || option > 1) {

            cout << "You entered a wrong value. Enter an option (0-1): ";
        }
        else {
            cout << "Enter an option (0-1): ";

        }

        cin >> option;

    } while (option < 0 || option > 1);

    return option;
}

int subMenu() {
    int opt = 1;

    do {
        // diplay the menu options
        cout << "******************** Customer Service Menu ********************" << endl;
        cout << "1) Place an order" << endl;
        cout << "2) Check an order status" << endl;
        cout << "3) Cancel an order" << endl;
        cout << "0) Exit" << endl;
        // read an option value

        if (opt < 0 || opt > 3) {

            cout << "You entered a wrong value. Enter an option (0-1): ";
        }
        else {
            cout << "Enter an option (0-3): ";

        }

        cin >> opt;

    } while (opt < 0 || opt > 3);

    return opt;
}

void customerService(Connection* conn, int customerId) {
    struct ShoppingCart cart[5];
    int checkedout = 0;
    int productCount;
    int orderId;
    int opt = 0;

    do {

        opt = subMenu();
        switch (opt) {

        case 1:

            cout << ">-------- Place an order ---------<";

            productCount = addToCart(conn, cart);
            displayProducts(cart, productCount);
            checkedout = checkout(conn, cart, customerId, productCount);
            if (checkedout) {
                cout << "The order is successfully completed." << endl;
                opt = 0;
            }
            else {
                cout << "The order is cancelled." << endl;
            }
            break;
        case 2:
            cout << ">-------- Check the order status --------<" << endl;
            cout << "Enter an order ID: ";
            cin >> orderId;
            displayOrderStatus(conn, orderId, customerId);
            break;
        case 3:

            cout << ">-------- Cancel an Order --------<" << endl;
            cout << "Enter an order ID: ";
            cin >> orderId;
            cancelOrder(conn, orderId, customerId);
            break;
        case 0:
            cout << "Back to main menu!..." << endl;
            break;

        }
    } while (opt != 0);

}

//Complete this function
//void displayOrderStatus(Connection* conn, int orderId, int customerId) {
//    Statement* stmt = nullptr;  // Initialize Statement pointer
//    int validOrderId = orderId;  // Used as IN OUT parameter for customer_order
//    string orderStatus;         // Used to store the order status
//
//    try {
//        // Step 1: Call customer_order to validate the order belongs to the customer
//        stmt = conn->createStatement("BEGIN customer_order(:1, :2); END;");
//        stmt->setInt(1, customerId);             // Input parameter for customerId
//        stmt->setInt(2, validOrderId);           // IN OUT parameter for orderId
//        stmt->registerOutParam(2, Type::OCCIINT); // Register OUT parameter
//        stmt->executeUpdate();
//
//        validOrderId = stmt->getInt(2);  // Get the updated orderId
//
//        // Check if the order is valid  
//        if (validOrderId == 0) {
//            cout << "Order ID is not valid." << endl;
//            conn->terminateStatement(stmt);  // Cleanup
//            return;
//        }
//
//        // Step 2: Call display_order_status to get the order status
//        conn->terminateStatement(stmt);  // Clean up previous statement
//        stmt = conn->createStatement("BEGIN display_order_status(:1, :2); END;");
//        stmt->setInt(1, orderId);               // Input parameter for orderId
//        stmt->registerOutParam(2, Type::OCCISTRING, 50); // Register OUT parameter for status
//        stmt->executeUpdate();
//
//        orderStatus = stmt->getString(2);  // Retrieve the order status
//
//        // Display the appropriate message based on the order status
//        if (orderStatus.empty()) {
//            cout << "Order does not exist." << endl;
//        }
//        else {
//            cout << "Order Status: " << orderStatus << endl;
//        }
//
//    }
//    catch (SQLException& ex) {
//        // Handle exceptions
//        cout << "Error: " << ex.getErrorCode() << " - " << ex.getMessage() << endl;
//    }
//
//    // Ensure cleanup of the statement object if it was created
//    if (stmt != nullptr) {
//        conn->terminateStatement(stmt);
//    }
//
//}
//void displayOrderStatus(Connection* conn, int orderId, int customerId) {
//    Statement* stmt = nullptr;  // Initialize Statement pointer
//    string orderStatus;         // Used to store the order status
//
//    try {
//        // Step 1: Validate if the order belongs to the customer using customer_order
//        stmt = conn->createStatement("BEGIN customer_order(:1, :2); END;");
//        stmt->setInt(1, customerId);              // Input parameter for customerId
//        stmt->setInt(2, orderId);                 // IN OUT parameter for orderId
//        stmt->registerOutParam(2, Type::OCCIINT); // Register OUT parameter
//        stmt->executeUpdate();
//
//        // Fetch the validated orderId
//        int validatedOrderId = stmt->getInt(2);  // Retrieve validated orderId
//
//        if (validatedOrderId == 0) {
//            cout << "Order ID is not valid." << endl;
//            conn->terminateStatement(stmt);      // Clean up
//            return;                              // Exit function
//        }
//
//        // Step 2: Fetch the order status using display_order_status
//        conn->terminateStatement(stmt);          // Clean up previous statement
//        stmt = conn->createStatement("BEGIN display_order_status(:1, :2); END;");
//        stmt->setInt(1, validatedOrderId);       // Input: validated order ID
//        stmt->registerOutParam(2, Type::OCCISTRING, 50); // Register OUT for status
//        stmt->executeUpdate();
//
//        // Retrieve the order status
//        orderStatus = stmt->getString(2);  // Fetch the second parameter: status
//
//        // Display the order status or error if not found
//        if (orderStatus.empty()) {
//            cout << "Order does not exist." << endl;
//        }
//        else {
//            cout << "Order Status: " << orderStatus << endl;
//        }
//
//    }
//    catch (SQLException& ex) {
//        // Handle SQL exceptions
//        cout << "Error: " << ex.getErrorCode() << " - " << ex.getMessage() << endl;
//    }
//
//    // Ensure cleanup of the statement object
//    if (stmt != nullptr) {
//        conn->terminateStatement(stmt);
//    }
//}
void displayOrderStatus(Connection* conn, int orderId, int customerId) {
    Statement* stmt = nullptr;  
    string orderStatus;         

    try {
        
        cout << "Debug: Input Customer ID: " << customerId << endl;
        cout << "Debug: Input Order ID: " << orderId << endl;

        // Step 1: Validate if the order belongs to the customer using customer_order
        stmt = conn->createStatement("BEGIN customer_order(:1, :2); END;");
        stmt->setNumber(1, Number(customerId));              
        stmt->setNumber(2, Number(orderId));                 
                // Register OUT parameter as NUMBER type
        
        
        stmt->executeUpdate();
        Number validatedOrderIdNum = stmt->getNumber(2);     // Retrieve the validated orderId as a Number
        int validatedOrderId = (int)validatedOrderIdNum;
        // Fetch the validated orderId
            // Convert the Number to an int using type casting
        cout << "Debug: Validated Order ID: " << validatedOrderId << endl;

        // Clean up the statement after use
        //conn->terminateStatement(stmt);
        //stmt = nullptr;

        // Check if the order is valid
        if (validatedOrderId == 0) {
            cout << "Order ID is not valid." << endl;
            return;
        }

        // Step 2: Fetch the order status using display_order_status
        stmt = conn->createStatement("BEGIN display_order_status(:1, :2); END;");
        stmt->setInt(1, validatedOrderId);                   // Input: validated order ID
        stmt->registerOutParam(2, Type::OCCISTRING, 50);     // Register OUT parameter for order status (string)
        stmt->executeUpdate();

        // Retrieve the order status
        orderStatus = stmt->getString(2);                    // Fetch the order status
        cout << "Debug: Order Status: " << orderStatus << endl;

        // Display the order status or error if not found
        if (orderStatus.empty()) {
            cout << "Order does not exist." << endl;
        }
        else {
            cout << "Order Status: " << orderStatus << endl;
        }
    }
    catch (SQLException& ex) {
        // Handle SQL exceptions
        cout << "Error: " << ex.getErrorCode() << " - " << ex.getMessage() << endl;
    }

    // Ensure cleanup of the statement object
    if (stmt != nullptr) {
        conn->terminateStatement(stmt);
    }
}

















//Complete this function
void cancelOrder(Connection* conn, int orderId, int customerId) {

    try {
        int returnedOrderId = 0;

        Statement* stmt = conn->createStatement("BEGIN customer_order(:1, :2); END;");
        stmt->setInt(1, customerId);
        stmt->setInt(2, orderId);

        stmt->execute();

        returnedOrderId = stmt->getInt(2);

        if (returnedOrderId == 0) {
            cout << "Order ID is not valid." << endl;
            conn->terminateStatement(stmt);
            return;
        }

        int cancelStatus = 0;

        stmt = conn->createStatement("BEGIN cancel_order(:1, :2); END;");
        stmt->setInt(1, orderId);
        stmt->registerOutParam(2, Type::OCCIINT);

        stmt->execute();


        cancelStatus = stmt->getInt(2);


        switch (cancelStatus) {
        case 1:
            cout << "The order has been already canceled." << endl;
            break;
        case 2:
            cout << "The order is shipped and cannot be canceled." << endl;
            break;
        case 3:
            cout << "The order is canceled successfully." << endl;
            break;
        default:
            cout << "An unexpected error occurred during cancellation." << endl;
            break;
        }

        conn->terminateStatement(stmt);
    }
    catch (SQLException& e) {
        cerr << "SQL Error: " << e.what() << endl;
    }
    catch (exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void createEnvironement(Environment* env) {
    try {
        env = Environment::createEnvironment(Environment::DEFAULT);
        cout << "environment created" << endl;
    }
    catch (SQLException& sqlExcp) {
        cout << "error";
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }

}

void teminateEnvironement(Environment* env) {
    Environment::terminateEnvironment(env);
}

void openConnection(Environment* env, Connection* conn, string user, string pass, string constr) {
    try {
        conn = env->createConnection(user, pass, constr);
    }
    catch (SQLException& sqlExcp) {
        cout << "error";
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }
}

void closeConnection(Connection* conn, Environment* env) {
    env->terminateConnection(conn);
}

int customerLogin(Connection* conn, int customerId) {

    Statement* stmt = nullptr;
    int found = 0;
    stmt = conn->createStatement("BEGIN find_customer(:1, :2); END;");
    stmt->setInt(1, customerId);
    stmt->registerOutParam(2, Type::OCCIINT, sizeof(found));
    stmt->executeUpdate();
    found = stmt->getInt(2);
    conn->terminateStatement(stmt);

    return found;

}

int addToCart(Connection* conn, struct ShoppingCart cart[]) {
    int product_id = 0;
    int productCount = 0;
    int addMore = 1;
    double price = 0;

    Product product;

    cout << "-------------- Add Products to Cart --------------" << endl;
    for (int i = 0; i < 5 && addMore == 1; i++) {
        do {
            cout << "Enter the product ID: ";
            cin >> product_id;

            // See if product ID exists in the shopping cart

            // call a stored procedure/function to check if the product exists
            //srore the returnning value in the price variable
            // price = findProduct(conn, cart[i].product_id);
            findProduct(conn, product_id, &product);

            /*if the price is zero, the product does not exist
              if the price is greater than zero the product display the following
              output:
              Product Price:*/

            if (product.price != 0) {
                cout << "Product Name: " << product.name << endl;
                cout << "Product Price: " << product.price << endl;
                cart[i].product_id = product_id;
                cart[i].name = product.name;
                cart[i].price = product.price;
            }
            else {
                cout << "The product does not exists. Try again..." << endl;
            }


        } while (product.price == 0);

        cout << "Enter the product Quantity: ";
        cin >> cart[i].quantity;

        productCount++;

        cout << "Enter 1 to add more products or 0 to checkout: ";
        cin >> addMore;

        while (addMore != 0 && addMore != 1) {
            cout << "Invalide input. Enter 1 to add more products or 0 to checkout: ";
            cin >> addMore;

        }


    }

    return productCount;

}

void findProduct(Connection* conn, int productId, struct Product* product) {
    Statement* stmt = nullptr;
    double found = 0;

    stmt = conn->createStatement("BEGIN find_product(:1, :2, :3); END;");
    stmt->setInt(1, productId);
    stmt->registerOutParam(2, Type::OCCIDOUBLE, sizeof(product->price));
    stmt->registerOutParam(3, Type::OCCISTRING, sizeof(product->name));
    stmt->executeUpdate();
    product->price = stmt->getDouble(2);
    product->name = stmt->getString(3);
    conn->terminateStatement(stmt);

}


void displayProducts(struct ShoppingCart cart[], int productCount) {

    double total = 0;
    cout << "------- Ordered Products ---------" << endl;
    for (int i = 0; i < productCount; i++) {
        cout << "---Item " << i + 1 << endl;
        cout << "Product ID: " << cart[i].product_id << endl;
        cout << "Name: " << cart[i].name << endl;
        cout << "Price: " << cart[i].price << endl;
        cout << "Quantity: " << cart[i].quantity << endl;
        total = total + cart[i].price * cart[i].quantity;
    }

    cout << "----------------------------------" << endl;
    cout << "Total: " << total << endl;

}

int checkout(Connection* conn, struct ShoppingCart cart[], int customerId, int productCount) {
    char confirm = ' ';
    int exit = 1;
    do {
        cout << "Would you like to checkout? (Y/y or N/n) ";
        cin >> confirm;
        if (confirm == 'Y' || confirm == 'y') {
            exit = 0;
        }
        else if (confirm == 'N' || confirm == 'n') {
            exit = 1;
        }
        else {
            cout << "Wrong input. Try again..." << endl;
        }
    } while (confirm != 'N' && confirm != 'n' && confirm != 'Y' && confirm != 'y');

    if (!exit) {
        Statement* stmt = nullptr;
        int order_id = 0;
        stmt = conn->createStatement("BEGIN add_order(:1, :2); END;");
        stmt->setInt(1, customerId);
        stmt->registerOutParam(2, Type::OCCIDOUBLE, sizeof(order_id));
        stmt->executeUpdate();
        order_id = stmt->getDouble(2);

        // Add items
        for (int i = 0; i < productCount; i++) {
            stmt->setSQL("BEGIN add_order_item(:1, :2, :3, :4, :5); END;");
            stmt->setInt(1, order_id);
            stmt->setInt(2, i + 1);
            stmt->setInt(3, cart[i].product_id);
            stmt->setInt(4, cart[i].quantity);
            stmt->setDouble(5, cart[i].price);
            stmt->executeUpdate();
        }

        conn->commit();
        conn->terminateStatement(stmt);

    }

    return !exit;
}



