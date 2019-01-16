package com.springboot.controller;

import com.springboot.entities.Wendu;
import com.springboot.service.WenduService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by 默默 on 2017/5/8.
 */
@RestController
public class WenduController {
    @Autowired
    private WenduService wenduService;
    @RequestMapping(value = "/findAllWendu")
    public Map<String,Object> findAllWendu(){
        Map<String,Object> wenduMap = new HashMap<String,Object>();
        List<Wendu> list = wenduService.findAllWendu();
        wenduMap.put("data",list);
        return wenduMap;
    }

    @RequestMapping(value = "/findAllShidu")
    public Map<String,Object> findAllShidu(){
        Map<String,Object> wenduMap = new HashMap<String,Object>();
        List<Wendu> list = wenduService.findAllShidu();
        wenduMap.put("data",list);
        return wenduMap;
    }
    @RequestMapping(value = "/addWendu")
    public Map<String,Object> addWendu(Double wendu,Double shidu) {
        Map<String, Object> wenduMap = new HashMap<String, Object>();
        System.out.println(wendu);
        if(wendu==null){
            wenduMap.put("cause","温湿度值不能为空");
            return wenduMap;
        }
        try {
          wenduService.saveWendu(wendu,shidu);
        } catch (Exception e) {
            wenduMap.put("status", "fail");
            return wenduMap;
        }
        wenduMap.put("status", "success");
        return wenduMap;
    }
}
