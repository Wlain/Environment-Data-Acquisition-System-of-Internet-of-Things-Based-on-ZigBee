package com.springboot.service;

import com.springboot.entities.Wendu;
import com.springboot.repositroy.WenduRepositoty;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.Date;
import java.util.List;

/**
 * Created by 默默 on 2017/5/8.
 */
@Service
public class WenduService{
   @Autowired
    private WenduRepositoty wenduRepositoty;
    public List<Wendu> findAllWendu(){
        List<Wendu> list =null;
        list=wenduRepositoty.findAllWendu();
        return list;
    }
    public List<Wendu> findAllShidu(){
        List<Wendu> list =null;
        list=wenduRepositoty.findAllShidu();
        return list;
    }
    public void saveWendu(Double wendu,Double shidu){
        Wendu wd = new Wendu();
        wd.setWendu(wendu);
        wd.setShidu(shidu);
        wd.setTime(new Date());
        wenduRepositoty.saveAndFlush(wd);
    }
}
