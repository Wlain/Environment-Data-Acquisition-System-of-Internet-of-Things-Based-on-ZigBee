package com.springboot.controller;

import com.springboot.entities.Guangzhao;
import com.springboot.entities.Wendu;
import com.springboot.service.GuangzhaoService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by 10497 on 2017/6/12.
 */
@RestController
public class GuangzhaoController {
    @Autowired
    private GuangzhaoService guangzhaoService;
    @RequestMapping("findGuangzhao")
    public Map<String,Object> findAllGuangzhao(){
        Map<String,Object> gzMap = new HashMap<String,Object>();
        List<Guangzhao> list = guangzhaoService.findAllGuangzhao();
        gzMap.put("data",list);
        return gzMap;
    }
    @RequestMapping("Guangzhao")
    public Map<String,Object> addGuangzhao(Integer gz){
        Map<String, Object> gzMap = new HashMap<String, Object>();
        //System.out.println(Guangzhao);
        if(gz==null){
            gzMap.put("cause","光照值不能为空");
            return gzMap;
        }
        try {
            guangzhaoService.addGuangzhao(gz);
        } catch (Exception e) {
            gzMap.put("status", "fail");
            return gzMap;
        }
        gzMap.put("status", "success");
        return gzMap;
    }

}
