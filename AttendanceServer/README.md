


```sql
-- 员工信息表
CREATE TABLE employee (
    employeeID INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(50) NOT NULL,
    sex CHAR(1),
    birthday DATE,
    address VARCHAR(255),
    phone VARCHAR(20),
    faceID INT UNIQUE,
    headfile VARCHAR(255)
);

-- 考勤表
CREATE TABLE attendance (
    attendanceID INT PRIMARY KEY AUTO_INCREMENT,
    employeeID INT NOT NULL,
    attTime DATETIME NOT NULL,
    notes VARCHAR(255),
    CONSTRAINT fk_attendance_employee FOREIGN KEY (employeeID) REFERENCES employee(employeeID) -- 外键关联员工信息表
);
```


