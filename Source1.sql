

SET SERVEROUTPUT ON 
--1.find_customer
CREATE OR REPLACE PROCEDURE find_customer(
    customer_id IN NUMBER,
    found OUT NUMBER
)
AS
BEGIN
    BEGIN
        -- Query to check if the customer exists
        SELECT 1 INTO found
        FROM customers
        WHERE customer_id = find_customer.customer_id;

    EXCEPTION
        -- No rows found
        WHEN no_data_found THEN
            found := 0;

        -- Multiple rows found
        WHEN too_many_rows THEN
            RAISE_APPLICATION_ERROR(-20001, 'Multiple customers found with the same ID');

        -- Other errors
        WHEN OTHERS THEN
            RAISE_APPLICATION_ERROR(-20002, 'An unexpected error occurred: ');
    END;
END;
/


--2.find_product 
CREATE OR REPLACE PROCEDURE find_product(
    productId IN NUMBER,
    price OUT products.list_price%TYPE,
    productName OUT products.product_name%TYPE
)
AS
    categoryId NUMBER;
    currentMonth VARCHAR2(20);
BEGIN
    -- Fetch product details and determine discount
    BEGIN
        -- Retrieve the product details
        SELECT product_name, list_price, category_id
        INTO productName, price, categoryId
        FROM products
        WHERE product_id = find_product.productId;

        -- Get the current month
        SELECT TO_CHAR(SYSDATE, 'Month') INTO currentMonth FROM DUAL;

        -- Apply 10% discount if the product is in category 2 or 5 and the month is November or December
        IF categoryId IN (2, 5) AND TRIM(currentMonth) IN ('November', 'December') THEN
            price := price * 0.9; -- Apply 10% discount
        END IF;

    EXCEPTION
        -- If no product is found
        WHEN no_data_found THEN
            productName := NULL;
            price := 0;

        -- If multiple rows are found (shouldn't happen, but a safety check)
        WHEN too_many_rows THEN
            RAISE_APPLICATION_ERROR(-20001, 'Multiple products found with the same ID');

        -- Handle other unexpected errors
        WHEN OTHERS THEN
            RAISE_APPLICATION_ERROR(-20002, 'An unexpected error occurred: ');
    END;
END;
/


--3.add_order
CREATE OR REPLACE PROCEDURE add_order(
    customer_id IN NUMBER,
    new_order_id OUT NUMBER
)
AS
BEGIN
    -- Generate a new order ID by calling the function generate_order_id
    new_order_id := generate_order_id();

    -- Insert the new order into the orders table
    INSERT INTO orders (
        order_id, 
        customer_id, 
        status, 
        salesman_id, 
        order_date
    )
    VALUES (
        new_order_id, 
        customer_id, 
        'Shipped', 
        56, 
        SYSDATE
    );

    COMMIT; -- Commit the transaction to save the changes
EXCEPTION
    WHEN OTHERS THEN
        RAISE_APPLICATION_ERROR(-20001, 'Error adding order: ');
END;
/

--4.generate_order_id
CREATE OR REPLACE FUNCTION generate_order_id
RETURN NUMBER
AS
    new_id NUMBER;
BEGIN
    -- Find the maximum order ID and increment it by 1
    SELECT NVL(MAX(order_id), 0) + 1 INTO new_id FROM orders;

    -- Return the new order ID
    RETURN new_id;
END;
/

DECLARE
    new_id NUMBER;
BEGIN
    add_order(101, new_id);
    DBMS_OUTPUT.PUT_LINE('New Order ID: ' || new_id);
END;
/


--5.add_order_item
CREATE OR REPLACE PROCEDURE add_order_item(
    orderId IN order_items.order_id%TYPE,
    itemId IN order_items.item_id%TYPE,
    productId IN order_items.product_id%TYPE,
    quantity IN order_items.quantity%TYPE,
    price IN order_items.unit_price%TYPE
)
AS
BEGIN
    -- Insert values into the order_items table
    INSERT INTO order_items (
        order_id,
        item_id,
        product_id,
        quantity,
        unit_price
    )
    VALUES (
        orderId,
        itemId,
        productId,
        quantity,
        price
    );

    COMMIT; -- Commit the transaction to save the changes
EXCEPTION
    WHEN OTHERS THEN
        RAISE_APPLICATION_ERROR(-20001, 'Error adding order item: ');
END;
/





--6.customer_order
CREATE OR REPLACE PROCEDURE customer_order(
    customerId IN NUMBER,
    orderId IN OUT NUMBER
)
AS
    matched_order_id NUMBER;
BEGIN
    -- Check if the order exists for the given customer
    BEGIN
        SELECT order_id
        INTO matched_order_id
        FROM orders
        WHERE customer_id = customer_order.customerId AND order_id = customer_order.orderId;

        -- If the order exists, pass the order ID back to the caller
        orderId := matched_order_id;

    EXCEPTION
        WHEN no_data_found THEN
            -- If no matching order is found, return 0
            orderId := 0;

        WHEN too_many_rows THEN
            -- Handle unexpected multiple rows (shouldn't occur with proper constraints)
            RAISE_APPLICATION_ERROR(-20002, 'Multiple orders found with the same ID for this customer');

        WHEN OTHERS THEN
            -- Catch other unexpected errors
            RAISE_APPLICATION_ERROR(-20001, 'Error checking customer order: ');
    END;
END;
//6.5 another one
CREATE OR REPLACE PROCEDURE customer_order(
    customerId IN NUMBER,
    orderId IN OUT NUMBER
)
AS
    matched_order_id NUMBER;
BEGIN
    BEGIN
        -- Match orderId and customerId
        SELECT order_id
        INTO matched_order_id
        FROM orders
        WHERE customer_id = customerId AND order_id = orderId;

        -- Set the OUT parameter
        orderId := matched_order_id;

    EXCEPTION
        WHEN no_data_found THEN
            
            orderId := 0;

        WHEN too_many_rows THEN
            -- Handle unexpected multiple rows (shouldn't occur with proper constraints)
            RAISE_APPLICATION_ERROR(-20002, 'Multiple orders found with the same ID for this customer');

        WHEN OTHERS THEN
            -- Handle unexpected errors
            RAISE_APPLICATION_ERROR(-20001, 'Error checking customer order: ');
    END;
END;
/




    



--7.display_order_status
CREATE OR REPLACE PROCEDURE display_order_status(
    orderId IN NUMBER,
    status OUT orders.status%TYPE
)
AS
BEGIN
    -- Query the status of the order
    BEGIN
        SELECT status
        INTO status
        FROM orders
        WHERE order_id = display_order_status.orderId;

    EXCEPTION
        WHEN no_data_found THEN
            -- If no order is found, set status to NULL
            status := NULL;

        WHEN OTHERS THEN
            -- Handle unexpected errors
            RAISE_APPLICATION_ERROR(-20001, 'Error retrieving order status: ');
    END;
END;
/





--8.cancel_order
CREATE OR REPLACE PROCEDURE cancel_order(
    orderId IN NUMBER,
    cancelStatus OUT NUMBER
)
AS
    orderStatus orders.status%TYPE;
BEGIN
    -- Check if the order exists and retrieve its status
    BEGIN
        SELECT status
        INTO orderStatus
        FROM orders
        WHERE order_id = orderId;

        -- Evaluate the current status of the order
        IF orderStatus = 'Canceled' THEN
            cancelStatus := 1; -- Already canceled
        ELSIF orderStatus = 'Shipped' THEN
            cancelStatus := 2; -- Shipped, cannot cancel
        ELSE
            -- Update status to 'Canceled'
            UPDATE orders
            SET status = 'Canceled'
            WHERE order_id = orderId;

            COMMIT; -- Save changes
            cancelStatus := 3; -- Successfully canceled
        END IF;

    EXCEPTION
        WHEN no_data_found THEN
            -- If the order ID does not exist, set cancelStatus to 0
            cancelStatus := 0;

        WHEN OTHERS THEN
            -- Handle unexpected errors
            RAISE_APPLICATION_ERROR(-20001, 'Error canceling order:');
    END;
END;
/
