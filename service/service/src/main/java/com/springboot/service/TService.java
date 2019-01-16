package com.springboot.service;

/**
 * Created by 默默 on 2017/6/10.
 */
import com.springboot.entities.Wendu;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.support.GenericXmlApplicationContext;

import java.net.*;
import java.io.*;
import java.sql.*;
import java.sql.Date;
import java.util.*;

public class TService {
 //  @Autowired
  // static WenduService wenduService;
 public static void add(Double wendu) {
     // TODO Auto-generated method stub
     Connection con =null;
     PreparedStatement ps =null;
     ResultSet rs =null;
     try {
         con = DBUtil.getConnection();
         String sql = "insert into wendu(wendu,time) value(?,?)";
         ps = con.prepareStatement(sql);
         ps.setDouble(1,wendu);
         java.util.Date date =new java.util.Date();
         ps.setDate(2,new Date( date.getTime()));
         ps.executeUpdate();
     } catch (SQLException e) {
         // TODO: handle exception
         e.printStackTrace();
     }finally{
         DBUtil.close(rs);
         DBUtil.close(ps);
         DBUtil.close(con);
     }
 }

    public static void main(String[] args) throws IOException{
        ServerSocket server=new ServerSocket(5678);

        while(true){
            Socket client=server.accept();
            BufferedReader in=new BufferedReader(new InputStreamReader(client.getInputStream()));
           // String str=in.readLine();
            //int str= in.read();
            String str = in.readLine();
            Double  wendu=Double.valueOf(str.toString());
            System.out.println(wendu);
            add(wendu);
            client.close();
        }
    }
}
