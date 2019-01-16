package com.springboot.service;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;

public class DBUtil {
    public static Connection getConnection(){
    	try {
			Class.forName("com.mysql.jdbc.Driver").newInstance();
		} catch (InstantiationException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (IllegalAccessException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (ClassNotFoundException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
        //String username ="com";
    	String User="root";
    	String Password ="123456";
   	    String url="jdbc:mysql://localhost:3306/test?user=test&password=test&useUnicode=true&characterEncoding=utf8";
    	Connection con =null;
    	try {
			con = DriverManager.getConnection(url, User, Password);
		} catch (SQLException e) {
			// TODO: handle exception
			e.printStackTrace();
		}
		return con;
    	}
    public static void close(Connection con){
    	try {
			if (con != null)
				con.close();
		} catch (SQLException e) {
			// TODO: handle exception
			e.printStackTrace();
		}
    }
    public static void close(java.sql.PreparedStatement ps){
    	try {
			if (ps != null)
				ps .close();
		} catch (SQLException e) {
			// TODO: handle exception
			e.printStackTrace();
		}
    }
    public static void close(ResultSet rs){
    	try {
			if (rs!= null)
				rs.close();
		} catch (SQLException e) {
			// TODO: handle exception
			e.printStackTrace();
		}
    }
    
}
