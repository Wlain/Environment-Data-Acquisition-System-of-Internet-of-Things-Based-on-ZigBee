package com.springboot.service;

import com.springboot.entities.User;
import com.springboot.repositroy.UserRepositoty;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

/**
 * Created by 默默 on 2017/5/6.
 */
@Service
public class UserService {
    @Autowired
    private UserRepositoty userRepositoty;
    public User findUserByName(String name){
        User user = null;
        try{
            user = userRepositoty.findByUserName(name);
        }catch (Exception e){}
    return user;
    }
}
